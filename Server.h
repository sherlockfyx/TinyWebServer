#pragma once

#include <memory>

#include "EventLoop.h"
#include "Channel.h"
#include "EventLoopThreadPool.h"
#include "config.h"

//Server有一个acceptChannel, 负责监听listen
class Server {
public:
    Server(EventLoop *loop, int threadNum, int port);
    ~Server() = default;

    EventLoop *getLoop() const {return loop_;}  //获得当前事件循环
    void start();
    void handNewConn();

    void handThisConn() {   //处理连接
        loop_->updatePoller(acceptChannel_);
    }
private:
    EventLoop *loop_;
    int threadNum_;

    std::unique_ptr<EventLoopThreadPool> eventLoopThreadPool_; 
    bool started_;
    SP_Channel acceptChannel_;

    int port_;
    int listenFd_;

};