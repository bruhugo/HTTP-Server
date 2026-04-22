#include <cstdint>
#include <functional>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

using namespace std;

namespace server {
namespace basic {

using Task = function<void()>;
using ThreadId = uint32_t;
enum class TPState {
    RUNNING, STOPPED
};

class ThreadPool {
public:
    ThreadPool(uint32_t threads = 10);
    ~ThreadPool();

    void Submit(Task task);
private:
    void threadEventLoop(ThreadId threadid);

    queue<Task> tasks;
    vector<thread> threads;
    condition_variable cv;
    mutex mu;
    TPState state;
};

} // basic
} // server


