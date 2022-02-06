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
#include "core/tingx_cycle.h"
#include "core/tingx_config.h"

using namespace std;
using namespace tingx;

bool exiting = false;

int main() {
    signal(SIGINT, [](int sig) { exiting = true; });

    Cycle core_cycle;

    core_cycle.AddListen(Ptr<Listen>(new Listen(65535)));
    Epoll epoll;

    cout << "server is listen on [";
    for (auto iter = core_cycle.GetOnListening().begin(), 
               end = core_cycle.GetOnListening().end(); iter != end; ++iter) {
        epoll.Add(iter->Get(), EpollEvent::Read);
        cout << static_cast<Socket*>(iter->Get())->GetPort();

        if(iter + 1 != end) 
            cout << ", ";
    }
    cout << "]" << endl;

    string recvbuf(1024, 0), sendbuf;
    for (;;) {
        int n = epoll.Wait(-1);
        auto on_ready = epoll.GetEventOnReady();
        for (int i = 0; i < n; ++i) {
            Socket* pDescriptor = static_cast<Socket*>(on_ready[i].data.ptr);
            if (core_cycle.IsOnListen(pDescriptor)) {
                Ptr<Connection> pConn{new Connection()};
                pConn->Accept(pDescriptor->Getfd());
                cout << "connect from: " << pConn->GetIpStr() << endl;

                epoll.Add(pConn, EpollEvent::Read);
                core_cycle.AddOpen(pConn);
            } else {
                ProcessStatus status = tingx_modules[0]->Process(pDescriptor);
                if (status == CLOSE) {
                    epoll.Del(pDescriptor);
                    core_cycle.DelOpen(pDescriptor);
                }
            }
        }

        if (exiting) break;
    }

    cout << endl << "exit..." << endl;

    return 0;
}