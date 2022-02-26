#pragma once

#include <condition_variable>
#include <mutex>

#include "noncopyable.h"

// 确保Thread中传入的func启动后才在start中返回

class CountDownLatch: noncopyable {
public: 
    explicit CountDownLatch(int count);

    ~CountDownLatch();

    void wait();

    void countDown();

private:
    mutable std::mutex mutex_;

    std::condition_variable cond_;

    int count_;
};