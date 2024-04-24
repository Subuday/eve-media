#include "Semaphore.h"
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>

Semaphore::Semaphore() : count(0) {}

void Semaphore::acquire() {
    std::unique_lock<std::mutex> lock(mtx);
    while (count == 0) {
        cv.wait(lock);
    }
    count--;
}

void Semaphore::release() {
    std::unique_lock<std::mutex> lock(mtx);
    count++;
    cv.notify_one();
}