#ifndef SRC_CORE_TINGX_SOCKET_HPP_
#define SRC_CORE_TINGX_SOCKET_HPP_

#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "core/tingx_descriptor.hpp"
#include "core/tingx_module.hpp"

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

    void SetConnectReciveModule(Module *module);
    Module* GetConnectReciveModule();
private:
    Module* connect_recive_;
};

class Connection : public Socket {
public:
    Connection();
    Connection(int fd, struct sockaddr_in &addr);

    int Connect();
    int Accept(Listen* listen);
    void SetHandlModule(Module* m) { handle_module_ = m; }
    Module* GetHandlModule() { return handle_module_; }

private:
    int Accept(int serv_fd);



    Module* handle_module_;
};

}


#endif