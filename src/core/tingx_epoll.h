#ifndef SRC_CORE_TINGX_EPOLL_H_
#define SRC_CORE_TINGX_EPOLL_H_

#include <vector>
#include <sys/epoll.h>
#include "core/tingx_descriptor.hpp"

namespace tingx {

enum EpollEvent { Read = EPOLLIN, Write = EPOLLOUT };

class Epoll : public Descriptor {
public:
    Epoll();
    ~Epoll();

    int Wait(int timeout);
    int Add(Descriptor *target, int events);
    int Del(Descriptor *target);

    std::vector<struct epoll_event>& GetEventOnReady();

private:
    std::vector<struct epoll_event> event_on_ready_;
};



}

#endif