#ifndef SRC_CORE_TINGX_CYCLE_H_
#define SRC_CORE_TINGX_CYCLE_H_

#include <vector>
#include <map>
#include <sys/epoll.h>
#include "core/tingx_refcount.hpp"
#include "core/tingx_module.hpp"

namespace tingx {

class Descriptor;

class Cycle {
public:
    Cycle();

    void AddOpen(Descriptor *pDescriptor);
    Descriptor* DelOpen(Descriptor* pDescriptor);
    Descriptor* DelOpen(int fd);
    bool AddListen(Descriptor* pDescriptor);
    bool IsOnListen(Descriptor* pDescriptor);
    bool IsOnListen(int fd);
    std::vector<Ptr<Descriptor>>& GetOnListening();

private:
    std::vector<Ptr<Descriptor>> all_opened_;
    std::vector<Ptr<Descriptor>> on_listening_;

};


enum EpollEvent { Read = EPOLLIN, Write = EPOLLOUT };

class Epoll : public Descriptor {
public:
    Epoll();
    ~Epoll();

    int Wait(int timeout);
    int Add(Descriptor *target, int events);
    int Del(Descriptor *target);

    std::vector<struct epoll_event>& GetEventOnReady();

private:
    std::vector<struct epoll_event> event_on_ready_;
};


class ConfigFileParser;

class CoreModule : public Module {
public:
    CoreModule(const char* name, ModuleType type, std::vector<Command>* com = nullptr);
    ProcessStatus Process(Descriptor* pDescriptor);

    // return the number of port which is occured an error while open
    int OpenListeningSocket();
    int Init(ConfigFileParser* parser);
    void MainLoop();
    void AddPort(int port) { all_port_.push_back(port); }
    Cycle& GetCoreCycle() { return core_cycle_; }
    Epoll& GetCoreEpoll() { return core_epoll_; }
    void SetExit(bool val) { is_exit_ = val; }

    std::vector<int>& GetAllPort() { return all_port_; }
    std::map<int, int>& GetPort2Index() { return port2index_; }
    std::map<std::string, int>& GetName2Index() { return name2index_; }

private:
    std::vector<int> all_port_;
    std::map<int, int> port2index_;
    std::map<std::string, int> name2index_;

    Cycle core_cycle_;
    Epoll core_epoll_;
    bool is_exit_ = false;
};

}

#endif