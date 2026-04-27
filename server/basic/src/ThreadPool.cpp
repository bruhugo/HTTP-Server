#include "ThreadPool.hpp"

using namespace server::basic;

ThreadPool::ThreadPool(uint32_t t): 
state{TPState::RUNNING}{
    for (ThreadId id = 0; id < t; id++) {
        threads.emplace_back(std::thread([this, id]{
            threadEventLoop(id);
        }));
    }
}

ThreadPool::~ThreadPool(){
    {
        std::lock_guard<std::mutex> lk(mu);
        state = TPState::STOPPED;
    }

    cv.notify_all();

    for (std::thread& t : threads)
        if (t.joinable()) t.join();
}

void ThreadPool::submit(Task task) {
    {
        std::lock_guard<std::mutex> lock(mu);
        tasks.push(task);
    }
    cv.notify_one();
}

void ThreadPool::threadEventLoop(ThreadId id){
    for (;;) {
        std::unique_lock<std::mutex> lk(mu);
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