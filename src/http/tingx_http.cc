#include "core/tingx_socket.h"
#include "core/tingx_config.h"
#include "http/tingx_http.h"

namespace tingx {

Http::Http(const char *name, ModuleType type) : Module(std::string(name), type) {
    //tingx_modules.push_back(this);
}

ProcessStatus Http::Process(Descriptor* pDescriptor) {

    return OK;
}


}