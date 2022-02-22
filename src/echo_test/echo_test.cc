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
    Socket* pSocket = static_cast<Socket*>(pDescriptor);
    int len = recv(pSocket->Getfd(), nullptr, 0, MSG_PEEK);
    std::cout << "recived " << len << " bytes form client" << std::endl << recvbuf << std::endl;
    int n = read(pSocket->Getfd(), &recvbuf[0], recvbuf.length());

    if (n == 0) {
        std::cout << pSocket->GetIpStr() << " close" << std::endl;
        return CLOSE;
    } else {
        std::cout << pSocket->GetIpStr() << ">" << std::endl;
        for (int i = 0; i < n; i++)
            std::cout << recvbuf[i];
        std::cout << std::endl;
        write(pSocket->Getfd(), &recvbuf[0], recvbuf.length());
    }
    return OK;
}

static ModuleRegister<EchoTestModule> echo_test("EchoTest", ModuleType::NORMAL, nullptr);
