#include <string.h>
#include <sys/socket.h>
#include "core/tingx_utilies.h"

namespace tingx {

bool StringTool::EndWith(const std::string& str1, const std::string &end) {
    if (end.length() > str1.length())
        return false;
    int i1 = str1.length() - 1, i2 = end.length() - 1;
    while (i1 >= 0 && i2 >= 0) {
        if (str1[i1] != end[i2])
            return false;
        i1--;
        i2--;
    }
    return true;
}

bool StringTool::EndWith(const std::string& str1, const char *end) {
    int i1 = str1.length() - 1, i2 = strlen(end) - 1;
    if (i2 > i1) return false;
    while (i1 >= 0 && i2 >= 0) {
        if (str1[i1] != end[i2])
            return false;
        i1--;
        i2--;
    }
    return true;
}


std::string StringTool::GetFileType(const std::string &str1) {
    int index = str1.rfind('.');
    if (index == std::string::npos)
        return std::string("");
    else return str1.substr(index + 1);
}


int Reader::Read(Descriptor *p) {
    if (pDescriptor == nullptr)
        return 0;

    buffer_.clear();
    int n = 0, all = 0;
    std::string temp(1024, 0);
    while((n = recv(p->Getfd(), &temp[0], 1024, MSG_DONTWAIT)) != 0) {
        all += n;
        buffer_.append(temp.substr(0, n));
    }
    return all;
}

int Reader::Read() {
    return Read(pDescriptor);
}

int Reader::Read(std::string &buffer, Descriptor *pDescriptor) {
    if (pDescriptor == nullptr)
        return 0;

    buffer.clear();
    int n = 0, all = 0;
    std::string temp(1024, 0);
    while((n = recv(pDescriptor->Getfd(), &temp[0], 1024, MSG_DONTWAIT)) != 0) {
        all += n;
        buffer.append(temp.substr(0, n));
    }
    return all;
}


}