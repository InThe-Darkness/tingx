#include "core/tingx_socket.h"
#include "core/tingx_utilies.h"
#include "core/tingx_config.h"
#include "http/tingx_http.h"
#include <iostream>
#include <fcntl.h>
namespace tingx {

// ---------------------------- Request -------------------------

std::string& Header::SerializeOut(std::string &buffer) {
    for (auto &iter : header) {
        buffer.append(iter.first);
        buffer.push_back(':');
        buffer.append(iter.second);
        buffer.append("\r\n");
    }
    buffer.append("\r\n");
    return buffer;
}

std::string& RequestHeader::SerializeOut(std::string &buffer) {
    buffer.append(method);
    buffer.push_back(' ');

    buffer.append(url);
    buffer.push_back(' ');

    buffer.append(version);
    buffer.push_back(' ');
    buffer.append("\r\n");


    return Header::SerializeOut(buffer);
}


std::map<int, const char *> ResponseHeader::ReasonPhrase {
    {200, "OK"},
    {404, "ERROR"},
    {500, "ERROR"}
};

std::string& ResponseHeader::SerializeOut(std::string &buffer) {
    buffer.append(version);
    buffer.push_back(' ');
    buffer.append(std::to_string(status_code));
    buffer.push_back(' ');
    buffer.append(ReasonPhrase[status_code]);
    buffer.append("\r\n");

    return Header::SerializeOut(buffer);
}


RequestHeader* RequestParser(std::string &buffer) {
    Ptr<RequestHeader> req = new RequestHeader();
    RequestParser(buffer, *req);
    return req.Detach();
}

RequestHeader* RequestParser(std::string &buffer, RequestHeader &req) {
    int i = 0, last = 0;
    enum Status { METHOD, URL, VERSION, HEADER };
    Status st = METHOD;
    while (i < buffer.length()) {
        while (buffer[i] != ' ' && buffer[i] != '\r') i++;

        switch (st) {
        case METHOD:
            req.method = buffer.substr(last, i - last);
            st = URL;
            break;
        case URL:
            req.url = buffer.substr(last, i - last);
            st = VERSION;
            break;
        case VERSION:
            req.version = buffer.substr(last, i - last);
            i++; // skip '\n'
            st = HEADER;
            break;
        case HEADER:
            int ks = last, split = i;
            while (buffer[i] != '\r') i++;
            req.header[buffer.substr(last, split - last)] = buffer.substr(split + 1, i - split - 1);
            i++; // skip '\n'
            break;
        }
        i++;
        last = i;
        if (buffer[i] == '\r') break;
    }
    return &req;
}


std::ostream& operator<< (std::ostream &os, RequestHeader &req) {
    os << "Method:" << req.method << std::endl;
    os << "URL:" << req.url << std::endl;
    os << "VERSION:" << req.version << std::endl;

    for (auto &iter : req.header) {
        os << iter.first << ":" << iter.second << std::endl;
    }
    return os;
}


ResponseHeader* ResponseHeader::BuildDefaultResponseHeader(RequestHeader *req) {
    Ptr<ResponseHeader> pRes {new ResponseHeader()};
    pRes->version = "HTTP/1.1";
    pRes->status_code = 200;
    if (StringTool::EndWith(req->url, ".html"))
        (*pRes)["Content-Type"] = "text/html";
    else if (StringTool::EndWith(req->url, ".js"))
        (*pRes)["Content-Type"] = "application/javascript";
    else if (StringTool::GetFileType(req->url).length() != 0)
        (*pRes)["Content-Type"] = "text/" + StringTool::GetFileType(req->url);
    else
        (*pRes)["Content-Type"] = "text/plain";
    return pRes.Detach();
}


File::File(const char *filename) {
    int n = stat(filename, &st_);
    int n2 = fd_ = open(filename, O_RDONLY);
    if (n == -1 || n2 == -1) {
        is_close_ = true;
        return;
    }
    is_close_ = false;
}

File::File(std::string &filename) {
    int n = stat(filename.c_str(), &st_);
    int n2 = fd_ = open(filename.c_str(), O_RDONLY);
    if (n == -1 || n2 == -1) {
        is_close_ = true;
        return;
    }
    is_close_ = false;
}

int File::Read(std::string &buffer, int n) {

    int l = 0;
    if (n == -1)
        l = read(fd_, &buffer[0], st_.st_size);
    else 
        l = read(fd_, &buffer[0], n);

    return l;
}

int File::GetFileSize() {
    return st_.st_size;
}

HttpModule::HttpModule(const char *name, ModuleType type, std::vector<Command>* com) :
        Module(std::string(name), type, com) {
}

ProcessStatus HttpModule::Process(Descriptor* pDescriptor) {
    std::string recvbuf(1024, 0);
    Socket* pSocket = static_cast<Socket*>(pDescriptor);
    int n = read(pSocket->Getfd(), &recvbuf[0], recvbuf.length());
    recvbuf[n] = '\0';

    if (n == 0) {
        return CLOSE;
    } else {
        Ptr<RequestHeader> req = RequestParser(recvbuf);
    

        std::string content_file = working_directory_;
        if (req->url.length() == 1 && req->url[0] == '/')
            req->url = std::string("/index.html");
        
        content_file.append(req->url);

        Ptr<ResponseHeader> pRes = ResponseHeader::BuildDefaultResponseHeader(req);
        File content(content_file);
        if (content.IsOpen()) {
            pRes->Insert("Content-Length", std::to_string(content.GetFileSize()));
            std::string send_buffer;
            pRes->SerializeOut(send_buffer);
            write(pSocket->Getfd(), &send_buffer[0], send_buffer.length());
            
            std::string content_buffer(content.GetFileSize(), 0);
            content.Read(content_buffer, -1);
            write(pSocket->Getfd(), &content_buffer[0], content_buffer.length());
        } else {
            pRes->status_code = 404;
//            pRes->Insert("Content-Length", 0);
            std::string send_buffer;
            pRes->SerializeOut(send_buffer);
            write(pSocket->Getfd(), &send_buffer[0], send_buffer.length());
        }

        
    }
    return CLOSE;
}

static ModuleRegister<HttpModule> http_module("http", ModuleType::CORE, nullptr);


}