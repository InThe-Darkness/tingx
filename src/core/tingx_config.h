#ifndef SRC_CORE_TINGX_CONFIG_HPP_
#define SRC_CORE_TINGX_CONFIG_HPP_

#include "core/tingx_module.hpp"
#include <vector>

namespace tingx {

extern int kDefaultListenNum;

extern int kDefaultMaxEpollEvent;

extern int kDefaultEpollTimeOut; // 100ms
extern int kDefaultWorkingThread;

extern std::vector<Module*>tingx_modules;

extern const char *conf_file;

}


#endif