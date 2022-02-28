#pragma once

#include <sys/epoll.h>
#include <unistd.h>

#include <string>
#include <cstring>
#include <unordered_map>
#include <map>
#include <memory>
#include <functional>

#include "Timer.h"
#include "config.h"
//枚举

enum ProcessState { //总过程 行， 头， 体
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

enum HttpMethod {   //方法
    METHOD_POST = 1,
    METHOD_GET,
    METHOD_HEAD 
};

enum HttpVersion {  //版本
    HTTP_10 = 1,
    HTTP_11 
};

class MimeType {    //媒体类型 
public:
    //对外接口
    static std::string getMime(const std::string &suffix);
private:
    //初始化
    static void init();
    static std::unordered_map<std::string, std::string> mime;   //媒体对应类型
    MimeType() = default;
    MimeType(const MimeType &m);

    static pthread_once_t once_control;
};



class HttpData: public std::enable_shared_from_this<HttpData> {
public:
    HttpData(EventLoop *loop, int connfd);
    ~HttpData( ) { close(fd_); }

    void reset();
    void seperateTimer();
    void setTimer(SP_TimerNode timer) {
        timer_ = timer;
    }
    SP_Channel getChannel() { return channel_; }
    EventLoop *getLoop() {return loop_;}
    void handleClose();
    void newEvent();

private:
    EventLoop *loop_;
    SP_Channel channel_;    //connChannel
    int fd_;
    bool error_;

    std::string inBuffer_;
    std::string outBuffer_;
 
    ConnectionState connectionState_;   //连接状态

    HttpMethod method_;
    HttpVersion HTTPVersion_;

    std::string fileName_;
    std::string path_;

    int nowReadPos_;

    ProcessState state_;//总状态
    ParseState hState_;//字符状态
    bool keepAlive_;
    std::unordered_map<std::string, std::string> headers_;
    std::weak_ptr<TimerNode> timer_;

    //事件处理函数
    void handleRead();
    void handleWrite();
    void handleConn();
    void handleError(int fd, int err_num, std::string short_msg);

    URIState parseURI();    //行
    HeaderState parseHeaders();     //头
    AnalysisState analysisRequest();    //分析请求
};