#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <thread>
#include <iostream>

#include "EventLoop.h"

// 线程对应的事件循环, 不同的线程事件循环调用结果会不同
thread_local EventLoop *loopInThisThread = nullptr;

//  创建异步唤醒描述符
int createEventfd() {
    int evfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evfd < 0) {
        std::cout<<"eventfd create failed"<<std::endl;
        abort();
    }
    return evfd;
}

//  每个事件循环会创建一个Epoll 和 一个wakeupChannel, 

//  wakeupChannel事件的处理函数在EventLoop中实现

//  对Channel的增删改会调用Epoll中的方法,  

EventLoop::EventLoop() :
    looping_(false),
    quit_(false),
    eventHandling_(false),
    callingPendingFunctors_(false),
    wakeupFd_(createEventfd()),
    threadId_(CurrentThread::getCacheTid()) {
    
    poller_ = std::make_shared<Epoll>(); 
    pwakeupChannel_ = std::make_shared<Channel>(this, wakeupFd_);

    if(loopInThisThread) {
        std::cout<<"Has Exist other thread run in this thread"<<std::endl;
    } else {
        loopInThisThread = this;
    }

    pwakeupChannel_->setEvents(EPOLLIN | EPOLLET);
    
    pwakeupChannel_->setReadHandler([this](){
        this->handleRead();
    });
    pwakeupChannel_->setConnHandler([this](){
        this->handleConn();
    });

    poller_->epoll_add(pwakeupChannel_, 0);
}

EventLoop::~EventLoop() {
    close(wakeupFd_);
    loopInThisThread = nullptr;
}

void EventLoop::quit() {
    quit_ = true;
    if(!isInLoopThread()) {
        wakeup();
    }
}

void EventLoop::wakeup() {  //异步唤醒
    uint64_t one = 1;
    ssize_t n = writen(wakeupFd_, (char*)(&one), sizeof(one));
    if(n != sizeof(one)) {
        std::cout<<"EventLoop::wakeup() writes " << n << " bytes instead of 8"<<std::endl;
    }
}

//事件处理函数
void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = readn(wakeupFd_, &one, sizeof one);
    if(n != sizeof(one)) {
        std::cout<<"EventLoop::handleRead() reads " << n << " bytes instead of 8" <<std::endl;
    }
    pwakeupChannel_->setEvents(EPOLLIN | EPOLLET);
}

void EventLoop::handleConn() {
    updatePoller(pwakeupChannel_, 0);
}


// 任务队列
void EventLoop::runInLoop(Callable &&cb) {
    if(isInLoopThread()) {
        cb();
    } else {
        queueInLoop(std::move(cb));
    }
        
}

//  主线程调用加入封装的HttpData(connfdChannel)
void EventLoop::queueInLoop(Callable&& cb) {    
    {
        std::lock_guard<std::mutex> lock(mutex_);
        pendingFunctors_.emplace_back(std::move(cb));
    }
    if(!isInLoopThread() || callingPendingFunctors_) {
        wakeup();
    }
        
}

// 核心
void EventLoop::loop() {
    assert(!looping_);
    assert(isInLoopThread());   //是否是创建循环的线程
    looping_ = true;    //开始
    quit_ = false;  //不退出

    std::vector<SP_Channel> ret;

    while(!quit_) {
        ret.clear();
        ret = poller_->poll();  //无事件, 阻塞一定时间
        eventHandling_ = true;
        for(auto &it : ret) 
             it->handleEvents();
        eventHandling_ = false;

        doPendingFunctors();        //处理任务队列 confdChannel HttpData 注册到事件树上
        poller_->handleExpired();   //处理过期连接
    }

    looping_ = false;
}

void EventLoop::doPendingFunctors() {
    std::vector<Callable> functors;
    callingPendingFunctors_ = true;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        functors.swap(pendingFunctors_);
    }
    for(size_t i = 0; i < functors.size(); i++) 
        functors[i]();
    callingPendingFunctors_ = false;
}
