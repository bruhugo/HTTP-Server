#pragma once

#include <functional>


namespace server {
namespace network {


class Context {

};

using Handler = std::function<void(Context)>;

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

}
}


