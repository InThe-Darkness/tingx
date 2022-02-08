#include "core/tingx_descriptor.hpp"
#include "core/tingx_parser.h"
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <fstream>
#include <map>

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


    ConfigFileParser parser("../conf/tingx.conf");
    
    parser.Parse();

    ParserObject* obj = parser.Get();
    string conf_buffer;
    ParserObject::SerializeOut(obj, conf_buffer);

    map<int, int> m;
    m[1] = 2;
    cout << m[1] << " " << m[2] << endl;

    cout << conf_buffer << endl;
    
    return 0;
}