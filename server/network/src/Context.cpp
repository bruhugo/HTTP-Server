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

Context::Context(){}

void Context::start(){
    if (state != ContextState::CREATED)
        throw std::runtime_error("Context already started");
        
    state = ContextState::REQUEST;
    if (curFilter){
        curFilter->handler(*this);
    }
    
    // Ensure we transition to RESPONSE if the chain ended or wasn't advanced
    if (state == ContextState::REQUEST) {
        state = ContextState::RESPONSE;
    }
}

void Context::next(){
    if (state != ContextState::REQUEST){
        throw std::runtime_error("next() called outside of request phase");
    }

    if (curFilter && curFilter->next) {
        curFilter = curFilter->next;
        curFilter->handler(*this);
    } else {
        // End of chain
        state = ContextState::RESPONSE;
    }
}

ContextBuilder::ContextBuilder(){}

Context& ContextBuilder::getContext(){
    return context;
}

ContextBuilder& ContextBuilder::setRespose(const Response& res){
    context.res = res;
    return *this;
}

ContextBuilder& ContextBuilder::setRequest(const Request& req){
    context.req = req;
    return *this;
}

ContextBuilder& ContextBuilder::setFilters(Filter* filter){
    context.curFilter = filter;
    return *this;
}

ContextBuilder& ContextBuilder::setParams(const Params& params){
    context.params = params;
    return *this;
}
