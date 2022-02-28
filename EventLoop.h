#pragma once

#include <assert.h>
#include <vector>
#include <memory>
#include <functional>
#include <mutex>

#include "base/Thread.h"
#include "base/CurrentThread.h"


#include "Epoll.h"
#include "Channel.h"
#include "Util.h"

#include "config.h"

// 除了mainReactor外, 其他事件循环都在对等线程中创建

//一个事件循环包含一个Epoll 和 wakeupChannel
class EventLoop {
public:
    EventLoop();

    ~EventLoop();

    void loop();
    void quit();

    void runInLoop(Callable &&cb);
    void queueInLoop(Callable &&cb);

    //是不是当前线程
    bool isInLoopThread() const {return threadId_ == CurrentThread::getCacheTid(); }    
    void assertInLoopThread() {
        assert(isInLoopThread());
    }

    //对外接口
    void shutdown(SP_Channel channel) {
        shutDownWR(channel->getFd());
    }
    void removeFromPoller(SP_Channel channel) {
        poller_->epoll_del(channel);
    }
    void updatePoller(SP_Channel channel, int timeout = 0) {
        poller_->epoll_mod(channel, timeout);
    }
    void addToPoller(SP_Channel channel, int timeout = 0) {
        poller_->epoll_add(channel, timeout);
    }
private:
    bool looping_;
    bool quit_;
    bool eventHandling_;
    bool callingPendingFunctors_;   // 互斥访问任务队列的布尔量
    
    int wakeupFd_;
    const pid_t threadId_;
    mutable std::mutex mutex_;

    std::vector<Callable> pendingFunctors_;
    SP_Epoll poller_;
    SP_Channel pwakeupChannel_;

    void wakeup();
    
    void handleRead(); 
    void handleConn();

    void doPendingFunctors();   
   
};
