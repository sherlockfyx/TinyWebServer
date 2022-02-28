#include "EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseloop, int numThreads) :
    baseLoop_(baseloop),
    started_(false),
    numThreads_(numThreads),
    next_(0)
{
    if(numThreads_ <= 0) {
        std::cout<<"numThreads_ <= 0"<<std::endl;
        abort();
    }
}

//Server类调用, 创建EventLoop线程开启事件循环, 之后主线程也开始事件循环
void EventLoopThreadPool::start() {     
    baseLoop_->assertInLoopThread();
    started_ = true;
    for(int i = 0; i < numThreads_; i++) {
        std::shared_ptr<EventLoopThread> t = std::make_shared<EventLoopThread>();
        threads_.push_back(t);
        //startLoop, 获得子线程的事件循环，如果没有阻塞等待子线程创建循环
        loops_.push_back(t->startLoop());
    }
}

//主线程Server调用, 获取下一个事件循环
EventLoop* EventLoopThreadPool::getNextLoop() {
    baseLoop_->assertInLoopThread();
    assert(started_);
    EventLoop *loop = baseLoop_;
    if(!loops_.empty()) {
        loop = loops_[next_];
        next_ = (next_ + 1) % numThreads_;
    }
    return loop;
}