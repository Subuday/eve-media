#include "Semaphore.h"
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>

Semaphore::Semaphore(int count) : _count(0) {}

void Semaphore::acquire() {
    std::unique_lock<std::mutex> lock(mtx);
    while (_count == 0) {
        cv.wait(lock);
    }
    _count--;
}

void Semaphore::release() {
    std::unique_lock<std::mutex> lock(mtx);
    _count++;
    cv.notify_one();
}