#include "core/tingx_socket.h"
#include "core/tingx_config.h"
#include "http/tingx_http.h"
#include <iostream>
namespace tingx {

Http::Http(const char *name, ModuleType type) : Module(std::string(name), type) {
    //tingx_modules.push_back(this);
}

ProcessStatus Http::Process(Descriptor* pDescriptor) {
    std::string recvbuf(1024, 0);
    Socket* pSocket = static_cast<Socket*>(pDescriptor);
    int n = read(pSocket->Getfd(), &recvbuf[0], recvbuf.length());

    if (n == 0) {
        return CLOSE;
    } else {
        std::string send_buffer(
            "HTTP/1.1 200 OK\r\n"
            "Content-Length: 33\r\n"
            "Content-Type: text/html\r\n"
            "\r\n"
            "<html><h1>hello world</h1></html>"
        );
        write(pSocket->Getfd(), &send_buffer[0], send_buffer.length());

    }
    return CLOSE;
}


}