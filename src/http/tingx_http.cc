#include "core/tingx_socket.h"
#include "core/tingx_config.h"
#include "http/tingx_http.h"
#include <iostream>
namespace tingx {

// ---------------------------- Request -------------------------

Request* HeaderParser(std::string &buffer) {
    Request* req = new Request();
    return HeaderParser(buffer, *req);
}

Request* HeaderParser(std::string &buffer, Request &req) {
    int i = 0, last = 0;
    enum Status { METHOD, URL, VERSION, HEADER };
    Status st = METHOD;
    while (i < buffer.length()) {
        while (buffer[i] != ' ') i++;

        switch (st) {
        case METHOD:
            req.method_ = buffer.substr(last, i - last);
            st = URL;
            break;
        case URL:
            req.url_ = buffer.substr(last, i - last);
            st = VERSION;
            break;
        case VERSION:
            req.version_ = buffer.substr(last, i - last);
            st = HEADER;
            break;
        case HEADER:
            int ks = last, split = i;
            while (buffer[i] != '\r') i++;
            req.header_[buffer.substr(last, split - last)] = buffer.substr(split + 1, i - split - 1);
            i++;
            break;
        }
        i++;
        if (buffer[i] == '\r') break;
    }
    return &req;
}







HttpModule::HttpModule(const char *name, ModuleType type, std::vector<Command>* com) :
        Module(std::string(name), type, com) {
    tingx_modules.push_back(this);
}

ProcessStatus HttpModule::Process(Descriptor* pDescriptor) {
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

HttpModule http_module("http", ModuleType::CORE, nullptr);


}