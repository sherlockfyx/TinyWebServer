#include <memory>
#include <errno.h>
#include <assert.h>
#include <vector>
#include <stdio.h>

#include <iostream>
#include <unistd.h>
#include <sys/syscall.h>
#include "Thread.h"

// std::hash<std::thread::id>{}(std::this_thread::get_id())
// CurrentThread定义, 线程局部信息, 不同的线程调用互不影响
namespace CurrentThread {
    thread_local pid_t              cacheTid = 0;               //tid
    thread_local char               tidString[32];              //tid -> string
    thread_local int                tidLength = 6;              //tid -> length
    thread_local const char*        threadName = "default";     //name
}

//cacheTid, tidString, tidLength
void CurrentThread::setCacheTid() { 
    if(cacheTid == 0) {
        cacheTid = static_cast<pid_t>(::syscall(SYS_gettid));
        tidLength = snprintf(tidString, sizeof(tidString), "%5d", cacheTid);
    }
}

//构造时线程还没开始运行, 只是进行初始化
Thread::Thread(const ThreadFunc& func, const std::string& name) : 
    started_(false),
    func_(func),
    tid_(0),
    name_(name),
    latch_(1) {
    setDefaultName();
}

void Thread::setDefaultName() {
    if(name_.empty()) {
        char buf[32];
        snprintf(buf, sizeof(buf), "Thread");
        name_ = buf;    
    }
}

//thread 线程函数

// void start_thread(std::vector<pid_t>& tid, Thread& obj) {
//   pid_t tmp_id = CurrentThread::tid();
//   {
//     std::unique_lock<std::mutex> lck(obj.t_mtx_);
//     tid.push_back(tmp_id);
//     obj.t_cov_.notify_all();
//   }
//   CurrentThread::t_threadName =
//       (obj.name_).empty() ? "Thread" : (obj.name_).c_str();
//   obj.func_();  //重写回调函数可以设置参数
// }


// 线程启动
void Thread::start() {
    assert(!started_);
    started_ = true;
    
    std::thread t([this](){
        this->tid_ = CurrentThread::getCacheTid();
        this->latch_.countDown();
        if(this->name_.empty()) {
            CurrentThread::threadName = "Thread";
        } else {
            CurrentThread::threadName = this->name_.c_str();
        }
        std::cout<<CurrentThread::cacheTid<<std::endl;
        this->func_();
        CurrentThread::threadName = "Finished";  
    });
    t.detach();

    latch_.wait();  //阻塞
    assert(tid_ > 0);
}

// void start_Thread(std::vector<pid_t>& tid, Thread& obj) {
//     pid_t tmp_id = CurrentThread::getCacheTid();
//     {
//         std::unique_lock<std::mutex> lck(obj.mutex_);
//         tid.push_back(tmp_id);
//         obj.cond_.notify_all();
//     }
//     CurrentThread::threadName = (obj.name_).empty() ? "Thread" : (obj.name_).c_str();
//     std::cout<<CurrentThread::threadName<<std::endl;
//     obj.func_();  //重写回调函数可以设置参数
// }

// void Thread::start() {
//   std::vector<pid_t> tid;
//   std::thread mythread(start_Thread, std::ref(tid), std::ref(*this));
//   started_ = true;
//   mythread.detach();

//   std::unique_lock<std::mutex> lck(mutex_);
//   cond_.wait(lck, [&] { return !tid.empty(); });
//   if (tid.empty()) {
//     perror("ipc error!\n");
//     exit(0);
//   }
//   tid_ = tid[0];
// }
