#include "core/tingx_config.h"
#include "core/tingx_utilies.h"
#include "core/tingx_module.hpp"
#include "http/tingx_http.h"

#include <fcntl.h>

using namespace tingx;

class GithubWebhook : public Module {
public:
    GithubWebhook(const char *name, ModuleType type, std::vector<Command>* com) : Module(name, type, com) {}
    virtual ProcessStatus Process(Descriptor* pDescriptor);
};

ProcessStatus GithubWebhook::Process(Descriptor* pDescriptor) {
    std::string recvbuf;
    int n = Reader::Read(recvbuf, pDescriptor);
    if (n == 0) {
        return CLOSE;
    }
    int fd = open("lastpost.txt", O_CREAT | O_WRONLY);
    write(fd, &recvbuf[0], recvbuf.length());
    close(fd);

    Ptr<Request> req = RequestParser(recvbuf);
    std::string secret_str = (req->header)["X-Hub-Signature-256"];

    Ptr<Response> response = Response::BuildDefault();
    response->header.status_code = 202;
    response->body = std::string("this is a test");

    Send(pDescriptor, response);

    return CLOSE;
}

static ModuleRegister<GithubWebhook> echo_test("GithubWebhook", ModuleType::NORMAL, nullptr);