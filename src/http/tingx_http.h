#ifndef SRC_HTTP_TINGX_HTTP_H_
#define SRC_HTTP_TINGX_HTTP_H_

#include "core/tingx_module.hpp"

namespace tingx {

class Http : public Module {
public:
    Http(const char *name, ModuleType type);
    virtual ProcessStatus Process(Descriptor* pDescriptor);

private:
    int SendHeader(Descriptor* pDescriptor);
    int SendBody(Descriptor* pDescriptor);
};



}

#endif