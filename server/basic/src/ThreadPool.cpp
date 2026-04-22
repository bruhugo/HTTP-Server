#include "../include/ThreadPool.hpp"

using namespace server::basic;

ThreadPool::ThreadPool(uint32_t t = 10): 
state{TPState::RUNNING}{
    for (ThreadId id = 0; id < t; id++) {
        threads.emplace_back(thread([this, id]{
            threadEventLoop(id);
        }));
    }
}

ThreadPool::~ThreadPool(){
    {
        lock_guard<mutex> lk(mu);
        state = TPState::STOPPED;
    }

    cv.notify_all();

    for (thread& t : threads)
        if (t.joinable()) t.join();
}

void ThreadPool::Submit(Task task) {
    {
        lock_guard<mutex> lock(mu);
        tasks.push(task);
    }
    cv.notify_one();
}

void ThreadPool::threadEventLoop(ThreadId id){
    for (;;) {
        unique_lock<mutex> lk(mu);
        cv.wait(lk, [&]{
            return state == TPState::STOPPED || !tasks.empty();
        });
        if (state == TPState::STOPPED) return;

        Task task = tasks.front();
        tasks.pop();
        lk.unlock();
        task();
    }
}