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
    Filters();
    Filters(const Filters& other);
    ~Filters();

    Filter* root;
    Filter* tail;
    size_t size;

    void add(const Filters* filters);
    void add(Handler handler);
};

enum class ContextState{
    REQUEST, RESPONSE, CREATED
};

class Context {
public:
    Context();

    Request req;
    Response res;
    Params params;

    void start();
    void next();

    Filter* curFilter;
    ContextState state{ContextState::CREATED};
};


class ContextBuilder {
public:
    ContextBuilder();

    Context& getContext();

    ContextBuilder& setRequest(const Request& request);
    ContextBuilder& setRespose(const Response& response);
    ContextBuilder& setParams(const Params& params);
    ContextBuilder& setFilters(Filter* filter);

private:
    Context context;
};

}
}