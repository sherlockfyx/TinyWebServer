
#include "EventLoopThread.h"
#include <functional>

// 含有事件循环的线程
EventLoopThread::EventLoopThread() : loop_(nullptr), existing_(false),
    thread_([this](){   //线程函数
        this->threadFunc();
    }, "EventLoopThread") {
}

EventLoopThread::~EventLoopThread() {
    existing_ = true;
    if(loop_ != nullptr) {
        loop_->quit();
    }
}


//主线程执行，启动对等线程 需要等到子线程启动循环，开始事件循环
EventLoop* EventLoopThread::startLoop() {   
    assert(!thread_.isStarted());
    thread_.start();    //开启
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this](){
            return this->loop_ != nullptr;
        });
    }
    return loop_;
}


//对等线程执行事件循环, 传给线程的func
void EventLoopThread::threadFunc() {    
    EventLoop loop;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        loop_ = &loop;
    }
    cond_.notify_all();
    loop.loop();    //核心 对等线程开始事件循环
    loop_ = nullptr;
}