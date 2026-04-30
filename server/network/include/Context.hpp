#pragma once

#include "Request.hpp"
#include "Response.hpp"

#include <functional>

namespace server {
namespace network {

class Context;

using Params = std::unordered_map<std::string, std::string>;
using Handler = std::function<void(Context&)>;

struct Filter {
    Handler handler;
    Filter* next;
};

class Filters {
public:
    ~Filters();

    Filter* root;
    Filter* tail;
    size_t size;

    void add(Filters* filters);
    void add(Handler handler);
};

enum class ContextState{
    REQUEST, RESPONSE, CREATED
};

class Context {
public:
    Request req;
    Response res;
    Params params;

    void start(){
        if (state != ContextState::CREATED)
            // TODO: replace with HTTP error later
            throw std::runtime_error("start cannot be called");
            
        state = ContextState::REQUEST;
        if (curFilter){
            curFilter->handler(*this);
        }
        state = ContextState::RESPONSE;
    }

    // A filter must call it in order to continue the filter chain
    void next(){
        if (state != ContextState::REQUEST){
            // TODO: replace with HTTP error later
            throw std::runtime_error("invalid start call");
        }

        curFilter = curFilter->next;
        if (curFilter){
            curFilter->handler(*this);
        }
        state = ContextState::RESPONSE;
    }

private:
    Filter* curFilter;
    ContextState state{ContextState::CREATED};
};


}
}