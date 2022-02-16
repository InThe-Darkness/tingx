#ifndef SRC_CORE_TINGX_UTILIES_H_
#define SRC_CORE_TINGX_UTILIES_H_

#include "core/tingx_module.hpp"
#include "core/tingx_parser.h"

namespace tingx {

Module* GetContainModel(ParserObject* obj);



class Reader {
public:
    Reader();


private:
    std::string buffer;
};

}

#endif