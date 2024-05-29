#include <chrono>
#include <iostream>
#include <Timer.h>

Timer::Timer(): worker(&Timer::work, this) {}

bool Timer::active() {
    lock_guard<mutex> lock(mtx);
    return internalActive();
}

void Timer::schedule(function<void()> task, int delay) {
    {
        lock_guard<mutex> lock(mtx);
        this->task = task;
        this->delay = delay;
        this->isReset = false;
    }
    cv.notify_all();
}

void Timer::reset() {
    {
        lock_guard<mutex> lock(mtx);
        isReset = true;
    }
    cv.notify_all();
}

void Timer::cancel() {
    {
        lock_guard<mutex> lock(mtx);
        this->task = nullptr;
        this->delay = 0;
        this->isReset = false;
    }
    cv.notify_all();
}

void Timer::work() {
    while (true) {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [this]() { return internalActive(); });
 
        auto endTime = chrono::steady_clock::now() + chrono::milliseconds(delay);
        while (internalActive() && !isReset && cv.wait_until(lock, endTime) != std::cv_status::timeout) {}

        if (isReset) {
            isReset = false;
            continue;
        }

        if (task) {
            task();
            task = nullptr;
        }
    }
}

bool Timer::internalActive() {
    return task != nullptr;
}