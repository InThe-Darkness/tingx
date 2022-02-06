#include "echo_test/echo_test.h"
#include "core/tingx_socket.h"
#include "core/tingx_config.h"
#include <iostream>
namespace tingx {

EchoTest::EchoTest(const char *name, ModuleType type) : Module(std::string(name), type) {
    tingx_modules.push_back(this);
}

ProcessStatus EchoTest::Process(Descriptor* pDescriptor) {
    std::string recvbuf(1024, 0);
    Socket* pSocket = static_cast<Socket*>(pDescriptor);
    int n = read(pSocket->Getfd(), &recvbuf[0], recvbuf.length());

    if (n == 0) {
        std::cout << pSocket->GetIpStr() << " close" << std::endl;
        return CLOSE;
    } else {
        std::cout << pSocket->GetIpStr() << ">" << std::endl;
        for (int i = 0; i < n; i++)
            std::cout << recvbuf[i];
        std::cout << std::endl;
        write(pSocket->Getfd(), &recvbuf[0], n);
    }
    return OK;
}

EchoTest echo_tes("EchoTest", ModuleType::NORMAL);

}