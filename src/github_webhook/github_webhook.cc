#include "core/tingx_config.h"
#include "core/tingx_utilies.h"
#include "core/tingx_module.hpp"
#include "http/tingx_http.h"

#include <fstream>
#include <iostream>

using namespace tingx;

class GithubWebhook : public Module {
public:
    GithubWebhook(const char *name, ModuleType type, std::vector<Command>* com) : Module(name, type, com) {}
    virtual ProcessStatus Process(Descriptor* pDescriptor);


    inline void SetScriptPath(std::string &path);
private:
    std::string script_path_;
};

ProcessStatus GithubWebhook::Process(Descriptor* pDescriptor) {
    std::string recvbuf;
    int n = Reader::Read(recvbuf, pDescriptor);
    if (n == 0) {
        return CLOSE;
    }

    std::ofstream outfile("lastpush.txt");
    if (outfile.is_open()) {
        outfile << recvbuf << std::endl;
        std::cout << "Write successful" << std::endl;
    } else {
        std::cout << "open file error" << std::endl;
    }

    Ptr<Response> response = Response::BuildDefault();
    response->header.status_code = 202;

    Ptr<Request> req = RequestParser(recvbuf);
    std::string secret_key = (req->header)["X-Hub-Signature-256"];

    if (script_path_.length() != 0) {
        int status = system(script_path_.c_str());
        if (status != 0) {
            response->body = std::string("script excute error");
            response->header.status_code = 500;
        } else {
            response->body = std::string("success");
            response->header.status_code = 202;
        }
    } else {
        response->body = std::string("script not found");
        response->header.status_code = 404;
    }

    Send(pDescriptor, response);

    return CLOSE;
}


inline void GithubWebhook::SetScriptPath(std::string &path) {
    script_path_ = path;
}


static int ScriptCommandHandler(Module *pModule, ParserObject *obj) {
    GithubWebhook *module = dynamic_cast<GithubWebhook*>(pModule);
    if (module == nullptr) {
        return -1;
    }
    
    KVItem *pKVItem = static_cast<KVItem*>(obj);
    String* pString = static_cast<String*>(pKVItem->second.Get());
    if (pString == nullptr) 
        return -1;

    module->SetScriptPath(pString->Get());

    return 0;
}


static std::vector<Command> github_webhook_module_commands {
    Command{ "script", 1, ScriptCommandHandler }
};


static ModuleRegister<GithubWebhook> echo_test("GithubWebhook", ModuleType::NORMAL, &github_webhook_module_commands);