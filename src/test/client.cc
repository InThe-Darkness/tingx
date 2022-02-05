#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<strings.h>

#include<iostream>

using namespace std;

bool exiting = false;

int main() {

    int serv_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    bzero(&serv_addr, addr_len);
    serv_addr.sin_port = 65535;
    serv_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr); 

    string recvbuf(1024, 0), sendbuf;

    if (connect(serv_fd, (struct sockaddr*)&serv_addr, addr_len) == 0) {
        cout << "connected..." << endl;
        for (;;) {
            cin >> sendbuf;
            write(serv_fd, &sendbuf[0], sendbuf.length());
            int n = read(serv_fd, &recvbuf[0], recvbuf.length());
            cout << "server>";
            for (int i = 0; i < n; i++)
                cout << recvbuf[i];
            cout << endl;
        }
    }

    return 0;
}