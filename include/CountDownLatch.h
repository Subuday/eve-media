#ifndef COUNT_DOWN_LATCH_H
#define COUNT_DOWN_LATCH_H

#include <mutex>
#include <condition_variable>
#include <iostream>

class CountDownLatch {
private:
    std::mutex mutex;
    std::condition_variable cv;
    int _count;

public:
    CountDownLatch(int count);

    void countDown();
    void await();
};

#endif // COUNT_DOWN_LATCH_H