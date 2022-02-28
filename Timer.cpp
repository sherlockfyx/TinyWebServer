#include <sys/time.h>

#include "Timer.h"

//chrono

TimerNode::TimerNode(SP_HttpData requestHttpData, int timeout) :
    deleted_(false),
    requestHttpData_(requestHttpData) {
        struct timeval now;
        gettimeofday(&now, nullptr);
        expiredTime_ = (((now.tv_sec % 10000)*1000) + (now.tv_usec / 1000)) + timeout;
}

TimerNode::TimerNode(const TimerNode& timerNode) {
    requestHttpData_ = timerNode.requestHttpData_;
}

//析构函数如何处理?????
TimerNode::~TimerNode() {
    if(requestHttpData_) {
        requestHttpData_->handleClose(); //Httpdata相关
    }
}

void TimerNode::update(int timeout) {
    //  更新定时器时间
    struct timeval now;
    gettimeofday(&now, nullptr);
    expiredTime_ = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000)) + timeout;
}

//判断定时器是否过期
bool TimerNode::isValid() {
    struct timeval now;
    gettimeofday(&now, nullptr);
    size_t diff = (((now.tv_sec % 10000) * 1000) + (now.tv_usec / 1000));
    if(diff < expiredTime_) {
        return true;
    } else {
        this->setDeleted();
        return false;
    }
}

//  HttpData对象由Server创建
//  释放HttpData, 节点还在队列中 reset 指针计数减一, TimerNode状态为删除
void TimerNode::clear() {
    requestHttpData_.reset(); 
    setDeleted();   
}

//TimerManager


// 定时器节点在此创建
void TimerManager::addTimer(SP_HttpData requestHttpData, int timeout) {
    SP_TimerNode newNode = std::make_shared<TimerNode> (requestHttpData, timeout);
    timerNodeQueue_.push(newNode);
    // 定时器节点已经管理了HttpData, 现在在HttpData中设置上级TimerNode
    requestHttpData->setTimer(newNode);
}

// 处理过期事件
void TimerManager::handleExpiredEvent() {
    while(!timerNodeQueue_.empty()) {
        SP_TimerNode curNode = timerNodeQueue_.top();
        if(curNode->isDeleted() || curNode->isValid() == false) {
            timerNodeQueue_.pop();
        } else {
            break;
        }
    }
}