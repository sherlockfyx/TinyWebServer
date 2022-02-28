#pragma once

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