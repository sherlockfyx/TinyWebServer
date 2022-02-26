#pragma once

#include <string>

#include <functional>
#include <memory>
#include <string>
#include <thread>

#include "CurrentThread.h"
#include "CountDownLatch.h"
#include "noncopyable.h"
#include "../config.h"


class Thread : noncopyable {
public:
    //常左值引用可以指向右值
    explicit Thread(const ThreadFunc& func, const std::string& name = std::string());
    ~Thread();

    void start();
    
    void join();

    bool isStarted() const { return started_; }

    std::thread::id getTid() const {return tid_; }

    std::string getName() const {return name_; }

private:
    void setDefaultName();

    bool started_;
    bool joined_;   

    std::thread thread_;
    // 线程局部信息
    size_t tid_;
    ThreadFunc func_;
    std::string name_;

    CountDownLatch latch_;  //门闩
};

