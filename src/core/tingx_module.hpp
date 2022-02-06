#ifndef SRC_CORE_TINGX_MODULE_H_
#define SRC_CORE_TINGX_MODULE_H_

#include <string>
#include "core/tingx_descriptor.hpp"

namespace tingx {

enum class ModuleType {CORE, HTTP, MAIL, NORMAL};

enum ProcessStatus {AGAIN, OK, ERROR, CLOSE};

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


protected:

    void SetIndex(int index) { index_ = index; }


private:
    std::string name_;
    ModuleType type_;
    int index_;

};

}

#endif