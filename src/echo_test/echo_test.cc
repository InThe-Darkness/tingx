#include "core/tingx_socket.h"
#include "core/tingx_config.h"
#include "core/tingx_utilies.h"
#include <iostream>
#include <sys/socket.h>


using namespace tingx;

class EchoTestModule : public Module {
public:
    EchoTestModule(const char *name, ModuleType type, std::vector<Command>* com) : Module(name, type, com) { }
    virtual ProcessStatus Process(Descriptor* pDescriptor);
};

ProcessStatus EchoTestModule::Process(Descriptor* pDescriptor) {
    std::string recvbuf(1024, 0);
    int n = read(*pDescriptor, &recvbuf[0], recvbuf.length());

    if (n == 0) {
        return CLOSE;
    } else {
        write(*pDescriptor, &recvbuf[0], recvbuf.length());
    }
    return OK;
}

static ModuleRegister<EchoTestModule> echo_test("EchoTest", ModuleType::NORMAL, nullptr);
