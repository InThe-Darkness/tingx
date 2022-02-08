#include "core/tingx_config.h"

namespace tingx {


const int kDefaultListenNum = 10;

const int kDefaultMaxEpollEvent = 100;

const int kDefaultEpollTimeOut = 100; // 100ms

std::vector<Module*>tingx_modules;

const char *conf_file = "../conf/tingx.conf";
}