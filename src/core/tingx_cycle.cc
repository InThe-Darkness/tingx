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

}