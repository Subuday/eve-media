#include <CountDownLatch.h>

CountDownLatch::CountDownLatch(int count) : _count(count) {}

void CountDownLatch::countDown() {
    std::lock_guard<std::mutex> lock(mutex);
    if (_count > 0) {
        _count--;
        if (_count == 0) {
            cv.notify_all();
        }
    }
}

void CountDownLatch::await() {
    std::unique_lock<std::mutex> lock(mutex);
    while (_count > 0) {
        cv.wait(lock);
    }
}