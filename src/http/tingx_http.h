#ifndef SRC_HTTP_TINGX_HTTP_H_
#define SRC_HTTP_TINGX_HTTP_H_

#include <map>
#include <string>
#include "core/tingx_module.hpp"

namespace tingx {

class Request {
public:
    Request() {]};
    ~Request() {]};

    std::string& operator[](const char *key) { return header_[key]; }
    std::string& operator[](std::string& key) { return header_[key]; };

public:
    std::map<std::string, std::string> header_;
    std::string method_;
    std::string url_;
    std::string version_;
    std::string body_buffer_;

};

class Response {
public:
    Response() {};
    ~Response() {};


public:
    std::string version_;
    int status_;
    std::map<std::string, std::string> head_;

    std::string body_buffer_;

};


class HttpModule : public Module {
public:
    HttpModule(const char *name, ModuleType type, std::vector<Command>* com);
    virtual ProcessStatus Process(Descriptor* pDescriptor);

private:
    int SendHeader(Descriptor* pDescriptor);
    int SendBody(Descriptor* pDescriptor);
};


}

#endif