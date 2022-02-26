#include "CountDownLatch.h"

CountDownLatch::CountDownLatch(int count) : count_(count) {

}

void CountDownLatch::wait() {   //阻塞直到count <= 0
    std::unique_lock<std::mutex> lock(mutex_);
    cond_.wait(lock, [this](){
        return this->count_ <= 0;
    });
}

void CountDownLatch::countDown() {  //count-- 
    std::lock_guard<std::mutex> lock(mutex_);
    count_--;
    if(count_ == 0) {
        cond_.notify_all();
    }
}