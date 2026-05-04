#include "Static.hpp"
#include "Error.hpp"

#include <fstream>
#include <sstream>

namespace server{
namespace network{

Handler staticFileHandler(
    std::string localpath, 
    std::string contentType
){
    return [localpath, contentType](Context& ctx){
        std::ifstream file(localpath);
        if (!file.is_open())
            throw HttpError::ServerError("unable to open file");

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();

        ctx.res.setStatusCode(200);
        ctx.res.headers.setContentType(contentType);
        ctx.res.setBody(content);

        file.close();
    };
}

}
}
