#pragma once

#include <cstdint>
#include <functional>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

namespace server {
namespace basic {

using Task = std::function<void()>;
using ThreadId = uint32_t;
enum class TPState {
    RUNNING, STOPPED
};

class ThreadPool {
public:
    ThreadPool(uint32_t threads = 10);
    ~ThreadPool();

    void submit(Task task);
private:
    void threadEventLoop(ThreadId threadid);

    std::queue<Task> tasks;
    std::vector<std::thread> threads;
    std::condition_variable cv;
    std::mutex mu;
    TPState state;
};

} // basic
} // server


