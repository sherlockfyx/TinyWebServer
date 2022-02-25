#pragma once

#include <sys/epoll.h>
#include <string>

#include "config.h"


// 一个Channel只管理一个 fd 的事件分发，connfdDChannel属于 HttpData 对象
// 主循环中的Server类含有一个 acceptChannel, 每个事件循环都有一个异步唤醒 pwakeupChannel
// 
class Channel {
public:
    Channel(EventLoop *loop);

    Channel(EventLoop *loop, int fd);

    ~Channel();

    int getFd();
    void setFd(int fd);

    //connfdChannel
    void setHolder(SP_HttpData holder) { holder_ = holder; }

    SP_HttpData getHolder() { return holder_.lock(); }

public:

    //设置事件回调函数:使用[](){}或std::bind, 所以是右值引用
    void setReadHandler(CallBack &&readHandler) {
        readHandler_ = readHandler;
    }
    void setWriteHandler(CallBack &&writeHandler) {
        writeHandler_ = writeHandler;
    }
    void setErrorHandler(CallBack &&errorHandler) {
        errorHandler_ = errorHandler;
    }
    void setConnHandler(CallBack &&connHandler) {
        connHandler_ = connHandler;
    }

    //处理对应的事件
    void handleRead();

    void handleWrite();

    void handleError(int fd, int err_num, std::string short_msg);   //错误处理

    void handleConn();

    //总的处理事件
    void handleEvents() {
        events_ = 0;
        if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
            events_ = 0;
            return ;
        }
        if(revents_ & EPOLLERR) {
            if(errorHandler_) errorHandler_();
            events_ = 0;
            return ;
        }

        if(revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
            handleRead();   // 处理读
        }
        if(revents_ & EPOLLOUT) {
            handleWrite();  //  处理写
        }
        handleConn();   //处理连接
    }


    //调用该函数设置想要的注册监听事件
    void setEvents(uint32_t ev) { events_ = ev; }   

    void setRevents(uint32_t ev) { revents_ = ev; } //返回

    uint32_t getEvents() { return events_; }        //获取注册事件

    uint32_t getLastEvents() { return lastEvents_; }

    bool EqualAndUpdateLastEvents() {
        bool res = (lastEvents_ == events_);  //判断两次注册事件是否相等，并更新
        lastEvents_ = events_;
        return res;
    }

private:
    EventLoop *loop_;
    int fd_;

    uint32_t events_;       //注册监听fd的事件
    uint32_t revents_;      //收到epoll_wait的事件
    uint32_t lastEvents_;   //上一次注册fd的事件

    //  connfdChannel 的持有者是 Httpdata,
    //  acceptChannel 的持有者是 Server,
    //wakeupfdChannel 的持有者是 EventLoop

    //  这里使用weak_ptr是因为 Httpdata 包含 connfdChannel, 会用shared_ptr指向connfdChannel
    //  connfdChannel用weak_ptr指向自己的上级
    WP_HttpData holder_;    
    

    //处理对应事件的回调函数
    CallBack readHandler_;
    CallBack writeHandler_;
    CallBack errorHandler_;     // ???
    CallBack connHandler_;
};