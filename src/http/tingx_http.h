#ifndef SRC_HTTP_TINGX_HTTP_H_
#define SRC_HTTP_TINGX_HTTP_H_

#include "core/tingx_module.hpp"

namespace tingx {

class HttpModule : public Module {
public:
    HttpModule(const char *name, ModuleType type, std::vector<Command>* com);
    virtual ProcessStatus Process(Descriptor* pDescriptor);

private:
    int SendHeader(Descriptor* pDescriptor);
    int SendBody(Descriptor* pDescriptor);
};



}

#endif