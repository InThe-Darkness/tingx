#include "core/tingx_config.h"

namespace tingx {


int kDefaultListenNum = 10;

int kDefaultMaxEpollEvent = 100;

int kDefaultEpollTimeOut = 100; // 100ms

int kDefaultWorkingThread = 20;

int kDefaultReadTimeout = 50;

std::vector<Module*>tingx_modules;

const char *conf_file = "../conf/tingx.conf";
}