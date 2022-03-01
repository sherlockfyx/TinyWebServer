#pragma once

#include <unistd.h>

#include <memory>
#include <queue>
#include <mutex>

#include "HttpData.h"

#include "config.h"

//  定时器节点, 对HttpData(connfdChannel)进行定时管理, 用shared_ptr
//  HttpData由Server创建, 同时HttpData包含定时器相关, 用weak_ptr
class TimerNode {
public:
    TimerNode(SP_HttpData requestHttpData, int timeout);
    TimerNode(const TimerNode& timerNode);//拷贝复制
    ~TimerNode();

    void update(int timeout); //更新定时器时间
    bool isValid();
    void clear();

    void setDeleted() {deleted_ = true; }
    bool isDeleted() const {return deleted_; }
    size_t getExpTime() const {return expiredTime_; }
    
private:
    bool deleted_;
    size_t expiredTime_;
    SP_HttpData requestHttpData_; 
};


struct TimerCmp {
    bool operator()(const SP_TimerNode& first, const SP_TimerNode& second) {
        return first->getExpTime() > second->getExpTime();
    }
};

//定时器管理器
class TimerManager {
public:
    TimerManager() = default;
    ~TimerManager() = default;
    void addTimer(SP_HttpData spHttpData, int timeout);
    void handleExpiredEvent();  //处理过期连接

private:
    std::priority_queue<SP_TimerNode, std::deque<SP_TimerNode>, TimerCmp> timerNodeQueue_;
};