#include "Static.hpp"
#include "Error.hpp"

#include <fstream>
#include <sstream>

namespace server{
namespace network{

Handler staticFileHandler(
    std::string localpath, 
    std::string contentType,
    LRUCache& cache
){
    return [localpath, contentType, &cache](Context& ctx){
        ctx.res.setStatusCode(200);
        ctx.res.headers.setContentType(contentType);

        auto ptr = cache.get(localpath);
        if (ptr){
            ctx.res.setBody(std::move(*ptr.get()));
            return;
        }
        
        std::ifstream file(localpath);
        if (!file.is_open())
            throw HttpError::ServerError("unable to open file");

        std::stringstream buffer;
        buffer << file.rdbuf();

        std::string content = buffer.str();
        ctx.res.setBody(content);
        cache.put(localpath, std::move(content));

        file.close();
    };
}

}
}
