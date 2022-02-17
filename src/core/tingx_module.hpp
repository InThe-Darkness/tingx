#ifndef SRC_CORE_TINGX_MODULE_H_
#define SRC_CORE_TINGX_MODULE_H_

#include <string>
#include <vector>
#include <functional>
#include "core/tingx_descriptor.hpp"

namespace tingx {

enum class ModuleType {CORE, HTTP, MAIL, NORMAL};

enum ProcessStatus {AGAIN, OK, ERROR, CLOSE};


#define CONF_TACK0 
#define CONF_TAKE1
#define CONF_TAKE2
#define CONF_TAKE3
#define CONF_TACK4
#define CONF_TACK5
#define CONF_TACK6
#define CONF_TACK7


class ParserObject;
class Module;

struct Command {
    std::string name;
    unsigned int arg_num;
    std::function<int(Module*, ParserObject*)> handler;
};


class ModuleCommandHandler {
public:
    ModuleCommandHandler() {};
    virtual int operator()(Module* pModule, ParserObject* obj) = 0;
};

class Module {
public:
    using ModuleClassName = Module;

    Module() = delete;
    Module(std::string& name, ModuleType type, std::vector<Command>* com = nullptr) : 
            name_(name), type_(type), commands_(com) {}
    Module(std::string &&name, ModuleType type, std::vector<Command>* com = nullptr) : 
            name_(name), type_(type), commands_(com) {}
    virtual ~Module() {}

    virtual ProcessStatus Process(Descriptor* pDescriptor) = 0;

    const std::string& GetName() { return name_; }
    ModuleType GetType() { return type_; }
    int GetIndex() { return index_; }
    void SetIndex(int index) { index_ = index; }

    std::string &GetWorkDir() { return working_directory_; }
    void SetWorkDir(std::string &str) { working_directory_ = str; }
    void SetWorkDir(std::string& str) { working_directory_ = str; }

    std::vector<Command>* GetCommands() { return commands_; }

    static int GetIndex(const std::vector<Module*>& con, const char *name)  {
        int index = -1, core_index = 0;
        for (int i = 0; i < con.size(); ++i) {
            if (con[i]->GetName() == name) {
                index = i;
            } else if (con[i]->GetName() == "core") {
                core_index = i;
            }
        }
        return index == -1 ? core_index : index;
    }

    friend class ModuleCommandHandler;
protected:
    std::vector<Command> *commands_ = nullptr;
    std::string name_;
    ModuleType type_;
    int index_;
    std::string working_directory_;
};



}

#endif