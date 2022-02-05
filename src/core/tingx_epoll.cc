#include "core/tingx_config.hpp"
#include "core/tingx_epoll.h"

namespace tingx {

Epoll::Epoll() : event_on_ready_(kDefaultMaxEpollEvent) {
    fd_ = epoll_create(10);
    is_close_ = false;
}


Epoll::~Epoll() {}

int Epoll::Wait(int timeout = kDefaultEpollTimeOut) {
    return epoll_wait(fd_, &event_on_ready_[0], kDefaultMaxEpollEvent, timeout);
}


int Epoll::Add(Descriptor *target, int events) {
    struct epoll_event ev;
    ev.data.ptr = target;
    ev.events = events;
    return epoll_ctl(fd_, EPOLL_CTL_ADD, target->Getfd(), &ev);
}

int Epoll::Del(Descriptor *target) {
    return epoll_ctl(fd_, EPOLL_CTL_DEL, target->Getfd(), nullptr);
}

std::vector<struct epoll_event>& Epoll::GetEventOnReady() {
    return event_on_ready_;
}


}
