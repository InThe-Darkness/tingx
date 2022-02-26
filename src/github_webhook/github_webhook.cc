#include "core/tingx_config.h"
#include "core/tingx_module.hpp"
#include "core/tingx_utilies.h"
#include <fcntl.h>

using namespace tingx;

class GithubWebhook : public Module {
public:
    GithubWebhook(const char *name, ModuleType type, std::vector<Command>* com) : Module(name, type, com) {}
    virtual ProcessStatus Process(Descriptor* pDescriptor);
};

ProcessStatus GithubWebhook::Process(Descriptor* pDescriptor) {
    std::string recvbuf(4096, 0);
    int n = read(*pDescriptor, &recvbuf[0], recvbuf.length());

    if (n == 0) {
        return CLOSE;
    } else {
        int fd = open("test.txt", O_CREAT | O_WRONLY);
        write(fd, &recvbuf[0], recvbuf.length());
    }
    return CLOSE;

}

static ModuleRegister<GithubWebhook> echo_test("GithubWebhook", ModuleType::NORMAL, nullptr);