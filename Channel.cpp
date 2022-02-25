#include "Channel.h"

// Channel 初始化只设置 loop和 fd ,后面的东西通过调用函数设置
Channel::Channel(EventLoop *loop) : loop_(loop), events_(0), lastEvents_(0) {

}

Channel::Channel(EventLoop *loop, int fd) : loop_(loop), fd_(fd), events_(0), lastEvents_(0) {

}

Channel::~Channel() {

}

int Channel::getFd() { return fd_; }

void Channel::setFd(int fd) { fd_ = fd; }

//处理对应的事件
void Channel::handleRead() {
    if(readHandler_) { readHandler_(); }
}

void Channel::handleWrite() {
    if(writeHandler_) { writeHandler_(); }
}

void Channel::handleConn() {
    if(connHandler_) { connHandler_(); }
}

