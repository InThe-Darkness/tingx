#include<signal.h>

#include<iostream>

#include "core/tingx_core.h"
#include "core/tingx_config.h"
#include "core/tingx_parser.h"
#include "core/tingx_socket.h"

using namespace std;
using namespace tingx;

bool exiting = false;

int main() {
    signal(SIGINT, [](int sig) {
        CoreModule* pCore = static_cast<CoreModule*>(tingx_modules[0]);
        pCore->SetExit(true);
    });

    int core_module_index = 0;
    for (int i = 0; i < tingx_modules.size(); ++i) {
        tingx_modules[i]->SetIndex(i);
        if (tingx_modules[i]->GetName() == "core")
            core_module_index = i;
    }
    if (core_module_index != 0) {
        Module* p = tingx_modules[0];
        tingx_modules[0] = tingx_modules[core_module_index];
        tingx_modules[core_module_index] = p;
        core_module_index = 0;
    }

    ConfigFileParser parser(conf_file);
    parser.Parse();

    CoreModule* core_module = static_cast<CoreModule*>(tingx_modules[0]);
    core_module->Init(&parser);
    core_module->OpenListeningSocket();

    Cycle &core_cycle = core_module->GetCoreCycle();

    cout << "server is listen on [";
    for (auto iter = core_cycle.GetOnListening().begin(), 
               end = core_cycle.GetOnListening().end(); iter != end; ++iter) {
        cout << static_cast<Socket*>(iter->Get())->GetPort();

        if(iter + 1 != end) 
            cout << ", ";
    }
    cout << "]" << endl;


    core_module->MainLoop();

    cout << endl << "exit..." << endl;

    return 0;
}