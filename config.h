#pragma once

#include <sys/epoll.h>
#include <functional>
#include <memory>


class Epoll;
class Channel;
class HttpData;
class TimerNode;
class EventLoop;
class CountDownLatch;

const int EVENTS_NUM = 4 * 1024;    //事件大小
const int EPOLLWAIT_TIME = 10000;   //timeout时间
const int MAXFDS = 100000;          //最大文件描述符数量
const int MAX_BUF = 4096;

const uint32_t DEFALT_EVENT = EPOLLIN | EPOLLET | EPOLLONESHOT;
const int DEFAULT_EXPIRED_TIME = 2000; // ms 
const int DEFAULT_KEEP_ALIVE_TIME = 5 * 60 * 1000; // ms 5min

using Callable = std::function<void()>;
using CallBack = std::function<void()>;
using ThreadFunc =  std::function<void()>;

using SP_Epoll = std::shared_ptr<Epoll>;
using SP_Channel =  std::shared_ptr<Channel>;

using SP_HttpData = std::shared_ptr<HttpData>;
using WP_HttpData = std::weak_ptr<HttpData>;

using SP_TimerNode = std::shared_ptr<TimerNode>;
using WP_TimerNode = std::weak_ptr<TimerNode>;

using SP_CountDownLatch = std::shared_ptr<CountDownLatch>;