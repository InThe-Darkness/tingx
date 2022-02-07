#ifndef SRC_CORE_TINGX_MODULE_H_
#define SRC_CORE_TINGX_MODULE_H_

#include <string>
#include <vector>
#include "core/tingx_descriptor.hpp"

namespace tingx {

enum class ModuleType {CORE, HTTP, MAIL, NORMAL};

enum ProcessStatus {AGAIN, OK, ERROR, CLOSE};


struct Command {
    std::string name;

};

class Module {
public:
    Module() = delete;
    Module(std::string& name, ModuleType type) : name_(name), type_(type) {}
    Module(std::string &&name, ModuleType type) : name_(name), type_(type) {}
    virtual ~Module() {}

    virtual ProcessStatus Process(Descriptor* pDescriptor) = 0;

    const std::string& GetName() { return name_; }
    ModuleType GetType() { return type_; }
    int GetIndex() { return index_; }

    std::vector<Command>& GetCommands() { return commands_; }

protected:

    void SetIndex(int index) { index_ = index; }


    std::vector<Command> commands_;


private:
    std::string name_;
    ModuleType type_;
    int index_;
};

}

#endif