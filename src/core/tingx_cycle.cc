#include "core/tingx_cycle.h"
#include "tingx_descriptor.hpp"
namespace tingx {

Cycle::Cycle() {}

void Cycle::AddOpen(Descriptor* pDescriptor) {
    all_opened_.push_back(pDescriptor);
}

Descriptor* Cycle::DelOpen(Descriptor* pDescriptor) {
    return DelOpen(pDescriptor->Getfd());
}

Descriptor* Cycle::DelOpen(int fd) {
    std::vector<Ptr<Descriptor>>::iterator iter = all_opened_.begin();
    for (iter; iter != all_opened_.end() && (*iter)->Getfd() != fd; ++iter);
    if (iter != all_opened_.end()) {
        all_opened_.erase(iter);
        return *iter;
    }
    return nullptr;
}


bool Cycle::AddListen(Descriptor* pDescriptor) {
    std::vector<Ptr<Descriptor>>::iterator iter = on_listening_.begin();
    for (; iter != on_listening_.end() && (*iter)->Getfd() != pDescriptor->Getfd(); ++iter);
    if (iter == on_listening_.end()) {
        on_listening_.push_back(pDescriptor);
        return true;
    }
    return false;
}


bool Cycle::IsOnListen(Descriptor *pDescriptor) {
    return IsOnListen(pDescriptor->Getfd());
}

bool Cycle::IsOnListen(int fd) {
    std::vector<Ptr<Descriptor>>::iterator iter = on_listening_.begin();
    for (; iter != on_listening_.end() && (*iter)->Getfd() != fd; ++iter);
    return iter != on_listening_.end();
}

std::vector<Ptr<Descriptor>>& Cycle::GetOnListening() {
    return on_listening_;
}

}