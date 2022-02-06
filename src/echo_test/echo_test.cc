#include "core/tingx_socket.h"
#include "core/tingx_config.h"
#include <iostream>


using namespace tingx;

class EchoTest : public Module {
public:
    EchoTest(const char *name, ModuleType type);
    virtual ProcessStatus Process(Descriptor* pDescriptor);
};

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

        std::string send_buffer(
            "HTTP/1.1 200 OK\r\n"
            "Content-Length: 33\r\n"
            "Content-Type: text/html\r\n"
            "\r\n"
            "<html><h1>hello world</h1></html>"
        );
        write(pSocket->Getfd(), &send_buffer[0], send_buffer.length());
    }
    return OK;
}

EchoTest echo_tes("EchoTest", ModuleType::NORMAL);