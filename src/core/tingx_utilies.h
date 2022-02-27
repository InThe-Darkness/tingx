#ifndef SRC_CORE_TINGX_UTILIES_H_
#define SRC_CORE_TINGX_UTILIES_H_

#include "core/tingx_module.hpp"
#include "core/tingx_parser.h"
#include "core/tingx_config.h"

namespace tingx {

Module* GetContainModel(ParserObject* obj);


template<typename T>
class ModuleRegister {
public:
    ModuleRegister() = delete;

    template<typename... Args>
    ModuleRegister(Args... args) {
        module_ = new T(args...);
        tingx_modules.push_back(module_);
    }

    ~ModuleRegister() {
        delete module_;
    }

private:
    T *module_;
};

class StringTool {
public:
    static bool EndWith(const std::string& str1, const std::string &end);
    static bool EndWith(const std::string& str1, const char *end);
    static std::string GetFileType(const std::string &str1);
};

class Reader {
public:
    Reader() : pDescriptor(nullptr) {};
    Reader(Descriptor *p) : pDescriptor(p) {}
    int Read(Descriptor *p);
    int Read();

    static int Read(std::string &buffer, Descriptor *pDescriptor);
    std::string *GetBuffer() { return &buffer_; }
private:
    std::string buffer_;
    Descriptor *pDescriptor;
};


// command slot

//int tingx_conf_string_slot(Module *pModule, ParserObject *obj);


}

#endif