#include <strings.h>
#include "core/tingx_config.h"
#include "core/tingx_socket.h"


namespace tingx {


Socket::Socket() {}

Socket::Socket(int domain, int type) {
    fd_ = socket(domain, type, 0);
    int opt = 1;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    is_close_ = false;
    type_ = type;
}

Socket::Socket(int fd, struct sockaddr_in &addr) {
    fd_ = fd;
    addr_ = addr;
    is_close_ = false;
}

Socket::~Socket() {}

int Socket::GetFamily() {
    return addr_.sin_family;
}

int Socket::GetType() {
    return type_;
}

int Socket::GetPort() {
    return ntohs(addr_.sin_port);
}

std::string& Socket::GetIpStr() {
    if (ip_str_.length() == 0)
        ip_str_ = Socket::GetIpStr(this);
    return ip_str_;
}

struct sockaddr_in& Socket::GetAddrStructure() {
    return addr_;
}

std::string Socket::GetIpStr(Socket *socket) {
    std::string ip(20, 0);
    inet_ntop(socket->addr_.sin_family, &(socket->addr_.sin_addr), &ip[0], ip.length());
    std::string ret(ip.c_str());
    ret.push_back(':');
    ret.append(std::to_string(socket->addr_.sin_port));
    return ret;
}


Listen::Listen(int port) : Socket(AF_INET, SOCK_STREAM) {
    bzero(&addr_, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port = htons(port);
    addr_.sin_addr.s_addr = htonl(INADDR_ANY);
    bind(fd_, (struct sockaddr*)&addr_, addr_len);
    listen(fd_, kDefaultListenNum);
}

void Listen::SetConnectReciveModule(Module* module) {
    connect_recive_ = module;
}

Module* Listen::GetConnectReciveModule() {
    return connect_recive_;
}


Connection::Connection() : Socket() {}

Connection::Connection(int fd, struct sockaddr_in &addr) : Socket(fd, addr) {}

int Connection::Connect() {
    return connect(fd_, (struct sockaddr*)&addr_, addr_len);
}

int Connection::Accept(int serv_fd) {
    if (!is_close_) {
        Close();
    }
    int ret = accept(serv_fd, (struct sockaddr*)&addr_, &addr_len);
    if (ret > 0) {
        fd_ = ret;
        is_close_ = false;
    }

    return ret;
}

int Connection::Accept(Listen *listen) {
    if (!is_close_) {
        Close();
    }
    int ret = Accept(listen->Getfd());
    if (ret > 0) {
        handle_module_ = listen->GetConnectReciveModule();
    }

    return ret;
}


}