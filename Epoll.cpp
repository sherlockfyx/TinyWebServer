#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <assert.h>

#include <cstring>

#include "Epoll.h"
// 对fd进行增，删，改的逻辑在Epoll中实现, 在Eventloop中被调用

// 创建事件树
Epoll::Epoll() : epollFd_(epoll_create1(EPOLL_CLOEXEC)), eEvents_(EVENTS_NUM) {
    assert(epollFd_ > 0);
}

Epoll::~Epoll() {
    close(epollFd_);
}


void Epoll::epoll_add(SP_Channel request, int timeout) {
    int fd = request->getFd();
    if(timeout > 0) {   //添加定时器
        addTimer(request, timeout);
        fdHttp_[fd] = request->getHolder();
    }
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.fd = fd;
    ev.events = request->getEvents();

    request->EqualAndUpdateLastEvents();    //更新epoll上最新的监听事件

    fdChan_[fd] = request;  //加入映射

    if(epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &ev) < 0) {
        perror("epoll_add failed!");
        fdChan_[fd].reset();    //在add之前fd会与Channel建立映射, 
    }
}

void Epoll::epoll_mod(SP_Channel request, int timeout) {    
    if(timeout > 0) {   //添加定时器
        addTimer(request, timeout);
    }
    int fd = request->getFd();
    //如果事件未发生改变， 直接返回
    if(!request->EqualAndUpdateLastEvents()) {
        struct epoll_event ev;
        bzero(&ev, sizeof(ev));
        ev.data.fd = fd;
        ev.events = request->getEvents();
        if(epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &ev) < 0) {
            perror("epoll_mod error!");
            fdChan_[fd].reset();
        }
    }
}

void Epoll::epoll_del(SP_Channel request) {       
    int fd = request->getFd();
    struct epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.fd = fd;
    ev.events = request->getLastEvents();

    if(epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &ev) < 0) {
        perror("epoll_del error!");
    }
    fdChan_[fd].reset();    
    fdHttp_[fd].reset();    //如果有的话connfdChannel
}

//返回活跃Channel集合
std::vector<SP_Channel> Epoll::poll() {
    std::vector<SP_Channel> activeChannels;
    int nfds = epoll_wait(epollFd_, eEvents_.data(), eEvents_.size(), EPOLLWAIT_TIME);
    if(nfds < 0) {
        //perror("epoll wait error!");
    } else {    
        for(int i = 0; i < nfds; i++) {
            int fd = eEvents_[i].data.fd;
            SP_Channel activeChannel = fdChan_[fd];
            if(activeChannel) {
                activeChannel->setRevents(eEvents_[i].events);
                activeChannel->setEvents(0);
                activeChannels.push_back(activeChannel);
            } else {
                perror("activeChannel is invaid!");
            }
        }
    }
    return activeChannels;
}

// 定时器相关, 在Timer中实现，由epoll调用
void Epoll::handleExpired() {
    timerManager_.handleExpiredEvent();
}

//底层在Timer中实现, 只有connfdChannel会添加定时器
void Epoll::addTimer(SP_Channel request, int timeout) {
    SP_HttpData t = request->getHolder();
    if(t) {
        timerManager_.addTimer(t, timeout);
    } else {
        perror("timer add failed!");
    }
}


