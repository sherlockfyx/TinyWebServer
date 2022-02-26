#pragma once

#include <string>

// 声明当前线程的相关信息
namespace CurrentThread {
    extern thread_local size_t             cacheTid;
    extern thread_local char               tidString[32];
    extern thread_local int                tidLength;
    extern thread_local std::string        threadName;
    
    void setCacheTid();

    inline size_t getCacheTid() {
        if(cacheTid == 0) {
            setCacheTid();
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