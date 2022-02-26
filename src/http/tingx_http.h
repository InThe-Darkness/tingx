#ifndef SRC_HTTP_TINGX_HTTP_H_
#define SRC_HTTP_TINGX_HTTP_H_

#include <map>
#include <string>
#include <fstream>
#include <sys/stat.h>
#include "core/tingx_module.hpp"
#include "core/tingx_refcount.hpp"

namespace tingx {

class Header : public RefCounted {
public:
    Header() {};
    virtual ~Header() {};

    std::string& operator[](const char *key) { return header[key]; }
    std::string& operator[](std::string& key) { return header[key]; };
    
    template<typename T1, typename T2>
    void Insert(T1 key, T2 val) {
        header.insert(std::make_pair(key, val));
    }

    std::string& SerializeOut(std::string &buffer);

public:
    std::map<std::string, std::string> header;

};


class RequestHeader : public Header {
public:
    std::string& SerializeOut(std::string &buffer);

public:
    std::string method;
    std::string url;
    std::string version;

};

std::ostream& operator<< (std::ostream &os, RequestHeader &req);


class ResponseHeader : public Header {
public:
    ResponseHeader() : version("HTTP/1.1") {}
    std::string& SerializeOut(std::string &buffer);

    static ResponseHeader* BuildDefaultResponseHeader(RequestHeader *req);

public:
    std::string version;
    int status_code;

    static std::map<int, const char *>ReasonPhrase;

};

class Request : public RefCounted {
public:
    RequestHeader header;
    std::string body;
};


class Response : public RefCounted {
public:
    Response(ResponseHeader *header_) : header(*header_) {} 

    static Response* BuildDefault() {
        Ptr<Response> res = new Response(ResponseHeader::BuildDefaultResponseHeader(nullptr));
        return res.Detach();
    }

    ResponseHeader header;
    std::string body;
};


Request* RequestParser(std::string &buffer);

int Send(Descriptor *pDescriptor, Response *response);



class File : public Descriptor {
public:
    File(const char *filename);
    File(std::string& filename);
    int Read(std::string &buffer, int n);
    int GetFileSize();
    bool IsOpen() { return !is_close_; }

public:


private:
    std::fstream file_;
    struct stat st_;
};



class HttpModule : public Module {
public:
    HttpModule(const char *name, ModuleType type, std::vector<Command>* com);
    virtual ProcessStatus Process(Descriptor* pDescriptor);

protected:
    int SendHeader(Descriptor* pDescriptor);
    int SendBody(Descriptor* pDescriptor);
};


}

#endif