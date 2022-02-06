#ifndef SRC_ECHO_TEST_ECHO_TEST_H_
#define SRC_ECHO_TEST_ECHO_TEST_H_

#include "core/tingx_module.hpp"

namespace tingx {

class EchoTest : public Module {
public:
    EchoTest(const char *name, ModuleType type);
    virtual ProcessStatus Process(Descriptor* pDescriptor);


};


}

#endif