#include <atomic>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <thread>

using namespace std;

class Timer {
    private:
        mutex mtx;
        condition_variable cv;
        thread worker;
        function<void()> task;
        int delay;
        bool isReset;

        void work();
        bool internalActive();
    public:
        Timer();
        bool active();
        void schedule(function<void()> task, int delay);
        void reset();
        void cancel();
};