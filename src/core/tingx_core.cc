#include "core/tingx_core.h"
#include "core/tingx_config.h"
#include "core/tingx_descriptor.hpp"
#include "core/tingx_parser.h"
#include "core/tingx_socket.h"

#include <exception>
#include <queue>
#include <iostream>

namespace tingx {

Cycle::Cycle() {}

void Cycle::AddOpen(Descriptor* pDescriptor) {
    all_opened_.push_back(pDescriptor);
}

Descriptor* Cycle::DelOpen(Descriptor* pDescriptor) {
    return DelOpen(pDescriptor->Getfd());
}

Descriptor* Cycle::DelOpen(int fd) {
    std::vector<Ptr<Descriptor>>::iterator iter = all_opened_.begin();
    for (iter; iter != all_opened_.end() && (*iter)->Getfd() != fd; ++iter);
    if (iter != all_opened_.end()) {
        all_opened_.erase(iter);
        return *iter;
    }
    return nullptr;
}


bool Cycle::AddListen(Descriptor* pDescriptor) {
    std::vector<Ptr<Descriptor>>::iterator iter = on_listening_.begin();
    for (; iter != on_listening_.end() && (*iter)->Getfd() != pDescriptor->Getfd(); ++iter);
    if (iter == on_listening_.end()) {
        on_listening_.push_back(pDescriptor);
        return true;
    }
    return false;
}


bool Cycle::IsOnListen(Descriptor *pDescriptor) {
    return IsOnListen(pDescriptor->Getfd());
}

bool Cycle::IsOnListen(int fd) {
    std::vector<Ptr<Descriptor>>::iterator iter = on_listening_.begin();
    for (; iter != on_listening_.end() && (*iter)->Getfd() != fd; ++iter);
    return iter != on_listening_.end();
}

std::vector<Ptr<Descriptor>>& Cycle::GetOnListening() {
    return on_listening_;
}

// ---------------------------- Epoll ---------------------------
Epoll::Epoll() : event_on_ready_(kDefaultMaxEpollEvent) {
    fd_ = epoll_create(10);
    is_close_ = false;
}


Epoll::~Epoll() {}

int Epoll::Wait(int timeout = kDefaultEpollTimeOut) {
    return epoll_wait(fd_, &event_on_ready_[0], kDefaultMaxEpollEvent, timeout);
}


int Epoll::Add(Descriptor *target, int events) {
    struct epoll_event ev;
    ev.data.ptr = target;
    ev.events = events;
    return epoll_ctl(fd_, EPOLL_CTL_ADD, target->Getfd(), &ev);
}

int Epoll::Del(Descriptor *target) {
    return epoll_ctl(fd_, EPOLL_CTL_DEL, target->Getfd(), nullptr);
}

std::vector<struct epoll_event>& Epoll::GetEventOnReady() {
    return event_on_ready_;
}


// ---------------------------- CoreModule ------------------------





CoreModule::CoreModule(const char *name, ModuleType type, std::vector<Command>* com) : Module(name, type, com) {
    tingx_modules.push_back(this);
}

ProcessStatus CoreModule::Process(Descriptor* pDescriptor) {
    std::string send_buffer("This is core module");
    write(pDescriptor->Getfd(), &send_buffer[0], send_buffer.length());
    return CLOSE;
}

int CoreModule::OpenListeningSocket() {
    int error = 0;
    for (auto port : all_port_) {
        try {
            Ptr<Listen> pListen(new Listen(port));
            core_cycle_.AddListen(pListen);
            core_epoll_.Add(pListen, EpollEvent::Read);
            pListen->SetConnectReciveModule(tingx_modules[port2index_[port]]);
        } catch (std::exception &e) {
            ++error;
        }
    }
    return error;
}



int CoreModule::Init(ConfigFileParser *parser) {
    for (auto iter : tingx_modules)
        name2index_[iter->GetName()] = iter->GetIndex();

    std::queue<ParserObject*> Queue, CoreConf;
    std::vector<ParserObject*> unresolved;
    Queue.push(parser->Get());

    while (!Queue.empty()) {
        ParserObject* obj = Queue.front();
        Queue.pop();

        if (obj->GetCatched()) 
            continue;

        ParserObject::Type obj_type = obj->GetType();
        if (obj_type == ParserObject::BLOCK || obj_type == ParserObject::ARRAY) {
            Block* pBlock = static_cast<Block*>(obj);
            for (auto iter : pBlock->Get())
                Queue.push(iter);
        } else {
            std::string module_name;
            String* pString;
            if (obj_type == ParserObject::STRING) {
                pString = static_cast<String*>(obj);
            } else {
                KVItem* pKVItem = static_cast<KVItem*>(obj);
                pString = static_cast<String*>(pKVItem->first.Get());
            }

            if (ParserObject::GetRoot(obj) == obj) {
                if (obj->GetType() == ParserObject::KVITEM && 
                        static_cast<KVItem*>(obj)->second->GetType() == ParserObject::BLOCK) {
                    module_name = pString->Get();
                    if (name2index_.find(module_name) == name2index_.end()) {
                        module_name = std::string("core");
                    } else {
                        Queue.push(static_cast<KVItem*>(obj)->second);
                        obj->SetCatched(true);
                        continue;
                    }
                } else {
                    module_name = std::string("core");
                }
            } else {
                module_name = static_cast<String*>(ParserObject::GetRoot(obj))->Get();
            }

            int module_index = name2index_[module_name];
            if (tingx_modules[module_index]->GetCommands() != nullptr) {
                for (auto iter : *(tingx_modules[module_index]->GetCommands())) {
                    if (iter.name == pString->Get()) {
                        iter.handler(tingx_modules[module_index], obj);
                        pString->SetCatched(true);
                        break;
                    }
                }
            }
            if (pString->GetCatched() == false) {
                CoreConf.push(obj);
            }
        }
    }

    while(!CoreConf.empty()) {
        ParserObject* obj = CoreConf.front();
        CoreConf.pop();

        String* pString;
        if (obj->GetType() == ParserObject::STRING) {
            pString = static_cast<String*>(obj);
        } else {
            KVItem* pKVItem = static_cast<KVItem*>(obj);
            pString = static_cast<String*>(pKVItem->first.Get());
        }   
        for (auto iter : *(tingx_modules[0]->GetCommands())) {
            if (iter.name == pString->Get()) {
                iter.handler(tingx_modules[0], obj);
                pString->SetCatched(true);
                break;
            }
        }

        if (obj->GetCatched() == false) 
            unresolved.push_back(obj);
    }

    if (unresolved.size() != 0)
        return -1;

    return 0;
}

void CoreModule::MainLoop() {
    for (;;) {
        int n = core_epoll_.Wait(-1);
        auto on_ready = core_epoll_.GetEventOnReady();
        for (int i = 0; i < n; ++i) {
            Socket* pDescriptor = static_cast<Socket*>(on_ready[i].data.ptr);
            if (core_cycle_.IsOnListen(pDescriptor)) {
                Ptr<Connection> pConn{new Connection()};
                Listen* pListen = static_cast<Listen*>(pDescriptor);
                pConn->Accept(pListen);
                pConn->SetHandlModule(pListen->GetConnectReciveModule());
                core_epoll_.Add(pConn, EpollEvent::Read);
                core_cycle_.AddOpen(pConn);
            } else {
                Connection* pConn = static_cast<Connection*>(pDescriptor);
                ProcessStatus status = pConn->GetHandlModule()->Process(pConn);
                if (status == CLOSE) {
                    core_epoll_.Del(pDescriptor);
                    core_cycle_.DelOpen(pDescriptor);
                }
            }
        }

        if (is_exit_) break;
    }


}


static int ListenCommandHandler(Module* pModule, ParserObject* obj) {
    CoreModule *real = static_cast<CoreModule*>(pModule);
    KVItem *pKVItem = static_cast<KVItem*>(obj);
    String* pString = static_cast<String*>(pKVItem->second.Get());
    if (pString == nullptr) 
        return -1;

    int port = atoi(pString->Get().c_str());
    real->AddPort(port);

    ParserObject* root = ParserObject::GetRoot(obj);
    int conn_recv_module_index = 0;
    if (root == obj) {
        conn_recv_module_index = 0;
    } else {
        String* pString = static_cast<String*>(root);
        conn_recv_module_index = real->GetName2Index()[pString->Get()];
    }
    
    real->GetPort2Index().insert(std::make_pair(port, conn_recv_module_index));

    return 0;
}

static int RootCommandHandler(Module* pModule, ParserObject* obj) {
    CoreModule *real = static_cast<CoreModule*>(pModule);
    KVItem *pKVItem = static_cast<KVItem*>(obj);
    String* pRootDir = static_cast<String*>(pKVItem->second.Get());
    if (pRootDir == nullptr) 
        return -1;

    ParserObject* root = ParserObject::GetRoot(obj);
    String* pString = static_cast<String*>(root);
    int model_index = real->GetName2Index()[pString->Get()];
    tingx_modules[model_index]->SetWorkDir(pString->Get());

    return 0;
}

std::vector<Command> core_module_commands {
    Command{ std::string("listen"), 1, ListenCommandHandler },
    Command{ std::string("root"), 1, RootCommandHandler },
};



CoreModule core_module("core", ModuleType::CORE, &core_module_commands);


}