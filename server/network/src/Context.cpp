#include "Context.hpp"

#include <stdexcept>

using namespace server::network;


Filters::Filters() : root(nullptr), tail(nullptr), size(0) {}

Filters::Filters(const Filters& other) : root(nullptr), tail(nullptr), size(0) {
    add(&other);
}

Filters::~Filters(){
    Filter* cur = root;
    while (cur != nullptr){
        auto prev = cur;
        cur = cur->next;
        delete(prev);
    }
}

void Filters::add(Handler handler){
    Filter* filter = new Filter();
    filter->handler = handler;

    ++size;

    if (root == nullptr){
        root = filter;
        tail = filter;
        return;
    }

    tail->next = filter;
    tail = tail->next;
}

void Filters::add(const Filters* filters){
    if (filters == nullptr) return;

    Filter* cur = filters->root;
    while (cur != nullptr){
        add(cur->handler);
        cur = cur->next;
    }
}