#ifndef SRC_CORE_TINGX_SOCKET_HPP_
#define SRC_CORE_TINGX_SOCKET_HPP_

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "core/tingx_descriptor.hpp"


namespace tingx {

class Socket : public Descriptor{
public:
    Socket();
    Socket(int domain, int type);
    Socket(int fd, struct sockaddr_in &addr);
    virtual ~Socket();

    int GetFamily();
    int GetType();
    int GetPort();
    std::string& GetIpStr();
    struct sockaddr_in& GetAddrStructure();

    static std::string GetIpStr(Socket *socket);

protected:
    struct sockaddr_in addr_;
    int type_;
    socklen_t addr_len = sizeof(struct sockaddr_in);
    std::string ip_str_;
};


class Listen : public Socket {
public:
    Listen(int port);
};

class Connection : public Socket {
public:
    Connection();
    Connection(int fd, struct sockaddr_in &addr);

    int Connect();
    int Accept(int serv_fd);
    int Accept(Listen* listen);
};

}


#endif