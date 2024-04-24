#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <iostream>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <vector>

class Semaphore {
private:
    std::mutex mtx;
    std::condition_variable cv;
    int _count;

public:
    Semaphore(int count);

    void acquire();
    void release();
};

#endif // SEMAPHORE_H