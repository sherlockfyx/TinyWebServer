#pragma once

#include "base/noncopyable.h" 
#include "EventLoopThread.h"

#include <memory>
#include <vector>
#include <iostream>

class EventLoopThreadPool: noncopyable {
public:
    EventLoopThreadPool(EventLoop *baseloop, int numThreads);
    ~EventLoopThreadPool() {
        std::cout<<"~EventLoopThreadPool()"<<std::endl;
    }
    void start();

    EventLoop *getNextLoop();
private:
    EventLoop *baseLoop_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<std::shared_ptr<EventLoopThread>> threads_;     //线程
    std::vector<EventLoop*> loops_;     //线程对应的事件循环
};