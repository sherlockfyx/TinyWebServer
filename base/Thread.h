#pragma once

#include <string>

#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "CurrentThread.h"
#include "CountDownLatch.h"
#include "noncopyable.h"
#include "../config.h"

// 构造函数只初始化调用函数, start才开始线程执行

class Thread : noncopyable {
public:
    //常左值引用可以指向右值
    explicit Thread(const ThreadFunc& func, const std::string& name = std::string());
    ~Thread()= default;

    void start();

    bool isStarted() const { return started_; }

    pid_t getTid() const {return tid_; }

    std::string getName() const {return name_; }

public:
    void setDefaultName();

    bool started_;  
    ThreadFunc func_;

    // 线程局部信息
    pid_t tid_;
    std::string name_;
    CountDownLatch latch_;  //门闩
};

