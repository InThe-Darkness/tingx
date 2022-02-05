#include<unistd.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<arpa/inet.h>
#include<signal.h>
#include<strings.h>
#include<stdio.h>

#include<iostream>
#include<vector>
#include<algorithm>
#include<map>

#include "core/tingx_descriptor.hpp"
#include "core/tingx_socket.h"
#include "core/tingx_epoll.h"

using namespace std;
using namespace tingx;

bool exiting = false;

int main() {
    signal(SIGINT, [](int sig) { exiting = true; });

    Ptr<Listen> pListen{new Listen(65535)};
    Ptr<Epoll> epoll{new Epoll()};
    epoll->Add(pListen, EpollEvent::Read);

    map<int, string> name;
    vector<Ptr<Descriptor>> opened;

    name[pListen->Getfd()] = Socket::GetIpStr(pListen);
    cout << "server is working on: " << name[pListen->Getfd()] << endl;

    string recvbuf(1024, 0), sendbuf;
    for (;;) {
        int n = epoll->Wait(-1);
        auto on_ready = epoll->GetEventOnReady();
        for (int i = 0; i < n; ++i) {
            Descriptor* pDescriptor = static_cast<Descriptor*>(on_ready[i].data.ptr);
            if (pDescriptor->Getfd() == pListen->Getfd()) {
                Ptr<Connection> pConn{new Connection()};
                pConn->Accept(pListen);
                string clnt_name = Socket::GetIpStr(pConn);
                cout << "connect from: " << clnt_name << endl;

                epoll->Add(pConn, EpollEvent::Read);
                name[pConn->Getfd()] = clnt_name;
                opened.push_back(pConn.Get());
            } else {
                int n = read(pDescriptor->Getfd(), &recvbuf[0], recvbuf.length());

                if (n == 0) {
                    cout << name[pDescriptor->Getfd()] << " close" << endl;
                    epoll->Del(pDescriptor);

                    vector<Ptr<Descriptor>>::iterator iter = opened.begin();
                    while(iter != opened.end() && (*iter)->Getfd() != pDescriptor->Getfd()) iter++;
                    opened.erase(iter);
                } else {
                    cout << name[pDescriptor->Getfd()] << ">";
                    for (int i = 0; i < n; i++)
                        cout << recvbuf[i];
                    cout << endl;
                    write(pDescriptor->Getfd(), &recvbuf[0], n);
                }
            }
        }

        if (exiting) break;
    }

    cout << endl << "exit..." << endl;

    return 0;
}