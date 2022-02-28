#pragma once

#include <string>

// 声明当前线程的相关信息
namespace CurrentThread {
    extern thread_local pid_t              cacheTid;
    extern thread_local char               tidString[32];
    extern thread_local int                tidLength;
    extern thread_local const char*        threadName;
    
    void setCacheTid();

    inline pid_t getCacheTid() {
        if(cacheTid == 0) {
            CurrentThread::setCacheTid();
        }
        return cacheTid;
    }

    inline const char* getTidString() {
        return tidString;
    } 

    inline int getTidLength() {
        return tidLength;
    }

    inline std::string getThreadName() {
        return threadName;
    }
}

