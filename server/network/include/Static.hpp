#pragma once

#include "Context.hpp"

#include <string>

namespace server{
namespace network{

Handler staticFileHandler(
    std::string localpath, 
    std::string contentType
);

}
}