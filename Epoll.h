#pragma once

#include <sys/epoll.h>

#include <memory>
#include <vector>
#include <unordered_map>

#include "Timer.h"
#include "Channel.h"
#include "HttpData.h"

#include "config.h"

//  一个事件循环会创建一个 Epoll, 监听该循环下所有的Channel

//  EventLoop会调用Epoll的函数对CHannel进行增删改

//  HttpData对象中会有一个 connfdChannel, Channel包含(accept, pwakeup, connfd)

//  含有一个定时器管理员, 管理定时器相关操作
class Epoll {
public:
    Epoll();

    ~Epoll();

    int getEpollFd() { return epollFd_; }

    void epoll_add(SP_Channel request, int timeout);

    void epoll_mod(SP_Channel request, int timeout);

    void epoll_del(SP_Channel request);

    //核心函数epoll_wait, 返回活跃事件的集合Channel
    std::vector<SP_Channel> poll();

    //为注册事件添加定时器
    void addTimer(SP_Channel request_data, int timeout);

    void handleExpired();   //处理过期事件, 具体实现在timerManager中

private:
    int epollFd_;

    std::vector<epoll_event> eEvents_; 

    std::unordered_map<int, SP_Channel> fdChan_;    //通过fd找到对应监听的Channel

    std::unordered_map<int, SP_HttpData> fdHttp_;   //找到HttpData(由Server创建)

    TimerManager timerManager_; //管理Node
};