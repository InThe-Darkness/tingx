#ifndef SRC_CORE_TINGX_CYCLE_H_
#define SRC_CORE_TINGX_CYCLE_H_

#include <vector>
#include "core/tingx_refcount.hpp"

namespace tingx {

class Descriptor;

class Cycle {
public:
    Cycle();

    void AddOpen(Descriptor *pDescriptor);
    Descriptor* DelOpen(Descriptor* pDescriptor);
    Descriptor* DelOpen(int fd);
private:
    std::vector<Ptr<Descriptor>> all_opened_;

};

}

#endif