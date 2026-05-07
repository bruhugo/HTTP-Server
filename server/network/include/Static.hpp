#pragma once

#include "Context.hpp"
#include "Cache.hpp"

#include <string>

namespace server{
namespace network{

Handler staticFileHandler(
    std::string localpath, 
    std::string contentType,
    LRUCache& cache
);

}
}