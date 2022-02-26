#include <memory>
#include <errno.h>
#include <assert.h>
#include <stdio.h>

#include "Thread.h"

//std::hash<std::thread::id>{}(std::this_thread::get_id())
// CurrentThread定义, 线程局部信息, 不同的线程调用互不影响
namespace CurrentThread {
    thread_local size_t             cacheTid = 0;               //tid
    thread_local char               tidString[32];              //tid -> string
    thread_local int                tidLength = 0;              //tid -> length
    thread_local std::string        threadName = "default";     //name
}


void CurrentThread::setCacheTid() { //cacheTid, tidString, tidLength
    if(cacheTid == 0) {
        cacheTid = std::hash<std::thread::id>{}(std::this_thread::get_id());
        tidLength = snprintf(tidString, sizeof(tidString), "%5d", cacheTid);
    }
}


//线程相关信息, 包括func_
struct ThreadData {
    ThreadFunc          func_;
    std::string         name_;
    size_t              tid_;
    SP_CountDownLatch   threadLatch_;

    ThreadData(const ThreadFunc& func, const std::string& name, size_t tid, SP_CountDownLatch threadLatch) : 
    func_(func),
    name_(name),
    tid_(tid),
    threadLatch_(threadLatch){

    }

    void runInThread () {
        tid_ = CurrentThread::getCacheTid();
        threadLatch_->countDown();  //释放
        if(name_.empty()) {
            CurrentThread::threadName = "Thread";
        }
        func_();
        CurrentThread::threadName = "finished";      
    }
 
};

void startThread() {

}

//===

//构造时线程还没开始运行, 只是进行初始化
Thread::Thread(const ThreadFunc& func, const std::string& name) : 
    started_(false),
    joined_(false),
    tid_(0),
    func_(func),
    name_(name),
    latch_(1) {
    setDefaultName();
}

Thread::~Thread() { // 线程开始但是没有join, 进行分离
    if(started_ && !joined_ && thread_.joinable()) {
        thread_.detach();
    }
}

void Thread::setDefaultName() {
    if(name_.empty()) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Thread");
        name_ = buf;    
    }
}

// 线程启动
void Thread::start() {
    assert(!started_);
    started_ = true;
    
    // ThreadData* data = new ThreadData(func_, name_, &tid_, &latch_);
    // 线程函数
    thread_ = std::thread([this](){
        this->tid_ = CurrentThread::getCacheTid();
        this->latch_.countDown();
        if(this->name_.empty()) {
            CurrentThread::threadName = "Thread";
        }
        this->func_();
        CurrentThread::threadName = "finished";  
    });
    latch_.wait();  //阻塞
    assert(tid_ > 0);
}

void Thread::join() {
    assert(started_);
    assert(!joined_);
    assert(thread_.joinable());
    joined_ = true;
    thread_.join();
}


