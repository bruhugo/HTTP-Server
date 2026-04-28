#include "Context.hpp"

#include <stdexcept>

using namespace server::network;


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

void Filters::add(Filters* filters){
    if (filters == nullptr) return;

    if (root == nullptr){
        root = filters->root;
    }

    tail = filters->tail;
    size += filters->size;
}