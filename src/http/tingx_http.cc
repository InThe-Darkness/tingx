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
    {202, "Accept"},
    {403, "Forbiden"},
    {404, "Not Found"},
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


Request* RequestParser(std::string &buffer) {
    Request *request = new Request();
    RequestHeader &header = request->header;
    int i = 0, last = 0;
    enum Status { METHOD, URL, VERSION, HEADER_KEY, HEADER_VAL };

    while(buffer[i] == ' ') i++;
    last = i;
    Status st = METHOD;
    std::string key, val;
    while (i < buffer.length()) {
        switch (st) {
        case METHOD:
            while (buffer[i] != ' ') i++;
            header.method = buffer.substr(last, i - last);
            st = URL;
            break;
        case URL:
            while (buffer[i] != ' ') i++;
            header.url = buffer.substr(last, i - last);
            st = VERSION;
            break;
        case VERSION:
            while (buffer[i] != '\r') i++;
            header.version = buffer.substr(last, i - last);
            i++; // skip '\n'
            st = HEADER_KEY;
            break;
        case HEADER_KEY:
            while (buffer[i] != ':' && buffer[i] != ' ') i++;
            key = buffer.substr(last, i - last);
            st = HEADER_VAL;
            while(buffer[i] == ':' || buffer[i] == ' ') i++;
            i--;
            break;
        case HEADER_VAL:
            while (buffer[i] != '\r') i++;
            val = buffer.substr(last, i - last);
            header[key] = val;
            i++; // skip '\n'
            st = HEADER_KEY;
        }
        i++;
        last = i;
        if (buffer[i] == '\r' && buffer[i + 1] == '\n') break;
    }
    i += 2;
    request->body = buffer.substr(i);
    return request;
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
    (*pRes)["Server"] = "tingx/0.0.1";
    (*pRes)["Connection"] = "close";

    pRes->status_code = 200;
    if (req == nullptr)
        return pRes.Detach();

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


int Send(Descriptor *pDescriptor, Response *response) {
    if (pDescriptor == nullptr || response == nullptr)
        return 0;
        
    std::string header_buffer;
    response->header.SerializeOut(header_buffer);
    int n, send = 0;
    n = write(*pDescriptor, &header_buffer[0], header_buffer.length());
    send += n;
    if (response->body.length() != 0) {
        n = write(*pDescriptor, &response->body[0], response->body.length());
        send += n;
    }
    return n;
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
    std::string recvbuf;
    int n = Reader::Read(recvbuf, pDescriptor);

    if (n == 0) {
        return CLOSE;
    } else {
        Ptr<Request> req = RequestParser(recvbuf);
        std::string &url = req->header.url;

        std::string content_file = working_directory_;
        if (url.length() == 1 && url[0] == '/')
            url = std::string("/index.html");
        
        content_file.append(url);

        Ptr<ResponseHeader> pRes = ResponseHeader::BuildDefaultResponseHeader(&req->header);
        File content(content_file);
        if (content.IsOpen()) {
            pRes->Insert("Content-Length", std::to_string(content.GetFileSize()));
            std::string send_buffer;
            pRes->SerializeOut(send_buffer);
            write(pDescriptor->Getfd(), &send_buffer[0], send_buffer.length());
            
            std::string content_buffer(content.GetFileSize(), 0);
            content.Read(content_buffer, -1);
            write(pDescriptor->Getfd(), &content_buffer[0], content_buffer.length());
        } else {
            pRes->status_code = 404;
//            pRes->Insert("Content-Length", 0);
            std::string send_buffer;
            pRes->SerializeOut(send_buffer);
            write(pDescriptor->Getfd(), &send_buffer[0], send_buffer.length());
        }

        
    }
    return CLOSE;
}

static ModuleRegister<HttpModule> http_module("http", ModuleType::CORE, nullptr);


}