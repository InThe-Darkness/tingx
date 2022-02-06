#include "core/tingx_descriptor.hpp"

#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <fstream>

using namespace tingx;
using namespace std;

int main() {

    std::string send_buffer(
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: 33\r\n"
        "Content-Type: text/html\r\n"
        "\r\n"
        "<html><h1>hello world</h1></html>"
    );

    cout << send_buffer.length() << endl;


    return 0;
}