#pragma once

#include <sys/epoll.h>
#include <unistd.h>

#include <string>
#include <cstring>
#include <unordered_map>
#include <map>
#include <memory>
#include <functional>
#include <mutex>

#include "Timer.h"
#include "config.h"

//HTTP方法, 版本
enum HttpMethod { METHOD_POST = 1, METHOD_GET, METHOD_HEAD };
enum HttpVersion { HTTP_10 = 1, HTTP_11 };

//处理过程
enum ProcessState { 
    STATE_PARSE_URI = 1,    
    STATE_PARSE_HEADERS,
    STATE_RECV_BODY,
    STATE_ANALYSIS,
    STATE_FINISH 
};

enum URIState {
    PARSE_URI_AGAIN = 1,
    PARSE_URI_ERROR,
    PARSE_URI_SUCCESS 
};

enum HeaderState {
    PARSE_HEADER_SUCCESS = 1,
    PARSE_HEADER_AGAIN,
    PARSE_HEADER_ERROR 
};

enum AnalysisState {
    ANALYSIS_SUCCESS = 1,
    ANALYSIS_ERROR 
};

enum ParseState {   //字符状态
    H_START = 0,
    H_KEY,
    H_COLON,
    H_SPACES_AFTER_COLON,
    H_VALUE,
    H_CR,
    H_LF,
    H_END_CR,
    H_END_LF 
};

enum ConnectionState { //连接状态
    H_CONNECTED = 0,
    H_DISCONNECTING,
    H_DISCONNECTED 
}; 


//媒体类型 
class MimeType {    
public:
    //对外接口
    static std::string getMime(const std::string &suffix);
private:
    //初始化
    MimeType() = default;
    MimeType(const MimeType &m);

    static void init(); //调用一次
    static std::unordered_map<std::string, std::string> mime;
    static std::once_flag flag_;
};



class HttpData: public std::enable_shared_from_this<HttpData> {
public:
    HttpData(EventLoop *loop, int connfd);
    ~HttpData() { close(connFd_); }

    void reset();
    void seperateTimer();
    void setTimer(SP_TimerNode timer) { timer_ = timer; }

    SP_Channel getChannel() { return connChannel_; }
    EventLoop *getLoop() {return loop_;}

    //connfdChannel 事件处理函数 注册到Epoll, 从Epoll中删除
    void newEvent();
    void handleClose();
 
private:
    EventLoop *loop_;           //所属子线程的事件循环
    SP_Channel connChannel_;    //connChannel
    WP_TimerNode timer_;        //上级的定时器

    int connFd_;
    bool error_;
    int nowReadPos_;
    bool keepAlive_;

    std::string inBuffer_;
    std::string outBuffer_;
    std::string fileName_;
    std::string path_;

    HttpMethod method_;
    HttpVersion HTTPVersion_;
    ProcessState state_;//总状态
    ParseState hState_;//字符状态
    ConnectionState connectionState_;   //连接状态

    //key->value
    std::unordered_map<std::string, std::string> headers_;

    URIState parseURI();    //行
    HeaderState parseHeaders();     //头
    AnalysisState analysisRequest();    //分析请求   

    //HttpData事件处理函数
    void handleRead();
    void handleWrite();
    void handleConn();
    void handleError(int fd, int err_num, std::string short_msg);


};