#ifndef SRC_CORE_TINGX_CONFIG_HPP_
#define SRC_CORE_TINGX_CONFIG_HPP_

#include "core/tingx_module.hpp"
#include <vector>

namespace tingx {

extern const int kDefaultListenNum;

extern const int kDefaultMaxEpollEvent;

extern const int kDefaultEpollTimeOut; // 100ms

extern std::vector<Module*>tingx_modules;

}


#endif