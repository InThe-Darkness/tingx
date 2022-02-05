#ifndef SRC_CORE_TINGX_DESCRIPTOR_HPP_
#define SRC_CORE_TINGX_DESCRIPTOR_HPP_

#include <unistd.h>

#include "core/tingx_refcount.hpp"

namespace tingx {

class Descriptor : public RefCounted {
public:
    Descriptor() : is_close_(true) {}
    Descriptor(int fd) : fd_(fd), is_close_(false) {}

    virtual ~Descriptor() {
        Close();
    }

    void Close() {
        if (is_close_) return;
        close(fd_);
        is_close_ = true;
    }

    int Getfd() { return fd_; }
    void Setfd(int fd) { fd_ = fd; }

    bool GetIsClose() { return is_close_; }
    void SetIsClose(bool val) { is_close_ = val; } 

    bool operator==(const Descriptor& other) {
        return other.fd_ == fd_;
    }

    bool operator==(int fd) {
        return fd == fd_;
    }

private:
    int fd_;
    bool is_close_;
};


}

#endif