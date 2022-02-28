#pragma once

#include <mutex>
#include <condition_variable>
#include "base/Thread.h"
#include "EventLoop.h"
#include "base/noncopyable.h"

// 封装一个事件循环和线程, 一个事件循环对应一个线程

// 事件循环在线程中创建

class EventLoopThread: noncopyable {
public: 
    EventLoopThread();
    
    ~EventLoopThread();

    EventLoop *startLoop();

private:
    void threadFunc();
    EventLoop *loop_;

    bool existing_; //是否退出 ? ? ?

    Thread thread_;
    std::mutex mutex_;
    std::condition_variable cond_;
};
