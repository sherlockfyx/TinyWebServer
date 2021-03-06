#include "HttpData.h"


#include <time.h>
#include "Channel.h"
#include "Util.h"
#include "EventLoop.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <iostream>
#include <string>


std::once_flag MimeType::flag_;
std::unordered_map<std::string, std::string> MimeType::mime;


// 图标
char favicon[555] = {
  '\x89', 'P', 'N', 'G', '\xD', '\xA', '\x1A', '\xA',
  '\x0', '\x0', '\x0', '\xD', 'I', 'H', 'D', 'R',
  '\x0', '\x0', '\x0', '\x10', '\x0', '\x0', '\x0', '\x10',
  '\x8', '\x6', '\x0', '\x0', '\x0', '\x1F', '\xF3', '\xFF',
  'a', '\x0', '\x0', '\x0', '\x19', 't', 'E', 'X',
  't', 'S', 'o', 'f', 't', 'w', 'a', 'r',
  'e', '\x0', 'A', 'd', 'o', 'b', 'e', '\x20',
  'I', 'm', 'a', 'g', 'e', 'R', 'e', 'a',
  'd', 'y', 'q', '\xC9', 'e', '\x3C', '\x0', '\x0',
  '\x1', '\xCD', 'I', 'D', 'A', 'T', 'x', '\xDA',
  '\x94', '\x93', '9', 'H', '\x3', 'A', '\x14', '\x86',
  '\xFF', '\x5D', 'b', '\xA7', '\x4', 'R', '\xC4', 'm',
  '\x22', '\x1E', '\xA0', 'F', '\x24', '\x8', '\x16', '\x16',
  'v', '\xA', '6', '\xBA', 'J', '\x9A', '\x80', '\x8',
  'A', '\xB4', 'q', '\x85', 'X', '\x89', 'G', '\xB0',
  'I', '\xA9', 'Q', '\x24', '\xCD', '\xA6', '\x8', '\xA4',
  'H', 'c', '\x91', 'B', '\xB', '\xAF', 'V', '\xC1',
  'F', '\xB4', '\x15', '\xCF', '\x22', 'X', '\x98', '\xB',
  'T', 'H', '\x8A', 'd', '\x93', '\x8D', '\xFB', 'F',
  'g', '\xC9', '\x1A', '\x14', '\x7D', '\xF0', 'f', 'v',
  'f', '\xDF', '\x7C', '\xEF', '\xE7', 'g', 'F', '\xA8',
  '\xD5', 'j', 'H', '\x24', '\x12', '\x2A', '\x0', '\x5',
  '\xBF', 'G', '\xD4', '\xEF', '\xF7', '\x2F', '6', '\xEC',
  '\x12', '\x20', '\x1E', '\x8F', '\xD7', '\xAA', '\xD5', '\xEA',
  '\xAF', 'I', '5', 'F', '\xAA', 'T', '\x5F', '\x9F',
  '\x22', 'A', '\x2A', '\x95', '\xA', '\x83', '\xE5', 'r',
  '9', 'd', '\xB3', 'Y', '\x96', '\x99', 'L', '\x6',
  '\xE9', 't', '\x9A', '\x25', '\x85', '\x2C', '\xCB', 'T',
  '\xA7', '\xC4', 'b', '1', '\xB5', '\x5E', '\x0', '\x3',
  'h', '\x9A', '\xC6', '\x16', '\x82', '\x20', 'X', 'R',
  '\x14', 'E', '6', 'S', '\x94', '\xCB', 'e', 'x',
  '\xBD', '\x5E', '\xAA', 'U', 'T', '\x23', 'L', '\xC0',
  '\xE0', '\xE2', '\xC1', '\x8F', '\x0', '\x9E', '\xBC', '\x9',
  'A', '\x7C', '\x3E', '\x1F', '\x83', 'D', '\x22', '\x11',
  '\xD5', 'T', '\x40', '\x3F', '8', '\x80', 'w', '\xE5',
  '3', '\x7', '\xB8', '\x5C', '\x2E', 'H', '\x92', '\x4',
  '\x87', '\xC3', '\x81', '\x40', '\x20', '\x40', 'g', '\x98',
  '\xE9', '6', '\x1A', '\xA6', 'g', '\x15', '\x4', '\xE3',
  '\xD7', '\xC8', '\xBD', '\x15', '\xE1', 'i', '\xB7', 'C',
  '\xAB', '\xEA', 'x', '\x2F', 'j', 'X', '\x92', '\xBB',
  '\x18', '\x20', '\x9F', '\xCF', '3', '\xC3', '\xB8', '\xE9',
  'N', '\xA7', '\xD3', 'l', 'J', '\x0', 'i', '6',
  '\x7C', '\x8E', '\xE1', '\xFE', 'V', '\x84', '\xE7', '\x3C',
  '\x9F', 'r', '\x2B', '\x3A', 'B', '\x7B', '7', 'f',
  'w', '\xAE', '\x8E', '\xE', '\xF3', '\xBD', 'R', '\xA9',
  'd', '\x2', 'B', '\xAF', '\x85', '2', 'f', 'F',
  '\xBA', '\xC', '\xD9', '\x9F', '\x1D', '\x9A', 'l', '\x22',
  '\xE6', '\xC7', '\x3A', '\x2C', '\x80', '\xEF', '\xC1', '\x15',
  '\x90', '\x7', '\x93', '\xA2', '\x28', '\xA0', 'S', 'j',
  '\xB1', '\xB8', '\xDF', '\x29', '5', 'C', '\xE', '\x3F',
  'X', '\xFC', '\x98', '\xDA', 'y', 'j', 'P', '\x40',
  '\x0', '\x87', '\xAE', '\x1B', '\x17', 'B', '\xB4', '\x3A',
  '\x3F', '\xBE', 'y', '\xC7', '\xA', '\x26', '\xB6', '\xEE',
  '\xD9', '\x9A', '\x60', '\x14', '\x93', '\xDB', '\x8F', '\xD',
  '\xA', '\x2E', '\xE9', '\x23', '\x95', '\x29', 'X', '\x0',
  '\x27', '\xEB', 'n', 'V', 'p', '\xBC', '\xD6', '\xCB',
  '\xD6', 'G', '\xAB', '\x3D', 'l', '\x7D', '\xB8', '\xD2',
  '\xDD', '\xA0', '\x60', '\x83', '\xBA', '\xEF', '\x5F', '\xA4',
  '\xEA', '\xCC', '\x2', 'N', '\xAE', '\x5E', 'p', '\x1A',
  '\xEC', '\xB3', '\x40', '9', '\xAC', '\xFE', '\xF2', '\x91',
  '\x89', 'g', '\x91', '\x85', '\x21', '\xA8', '\x87', '\xB7',
  'X', '\x7E', '\x7E', '\x85', '\xBB', '\xCD', 'N', 'N',
  'b', 't', '\x40', '\xFA', '\x93', '\x89', '\xEC', '\x1E',
  '\xEC', '\x86', '\x2', 'H', '\x26', '\x93', '\xD0', 'u',
  '\x1D', '\x7F', '\x9', '2', '\x95', '\xBF', '\x1F', '\xDB',
  '\xD7', 'c', '\x8A', '\x1A', '\xF7', '\x5C', '\xC1', '\xFF',
  '\x22', 'J', '\xC3', '\x87', '\x0', '\x3', '\x0', 'K',
  '\xBB', '\xF8', '\xD6', '\x2A', 'v', '\x98', 'I', '\x0',
  '\x0', '\x0', '\x0', 'I', 'E', 'N', 'D', '\xAE',
  'B', '\x60', '\x82',
};

void MimeType::init() {
    mime["default"] = "text/html";
    mime[".html"]   = "text/html";
    mime[".css"]    = "text/css";
    mime[".txt"]    = "text/plain";
    mime[".c"]      = "text/plain";
    mime[".bmp"]    = "image/bmp";
    mime[".png"]    = "image/png";
    mime[".jpg"]    = "image/jpeg";
    mime["ico"]     = "image/x-icon";
    mime[".gif"]    = "image/gif";
    mime[".avi"]    = "video/x-msvideo";
    mime[".mp3"]    = "audio/mp3";
    mime[".doc"]    = "application/msword";
    mime[".gz"]     = "application/x-gzip";
}

//返回媒体类型
std::string MimeType::getMime(const std::string &suffix) {
    std::call_once(MimeType::flag_, MimeType::init);
    if(mime.find(suffix) == mime.end()) {
        return mime["default"];
    } else {
        return mime[suffix];
    }
}


//构造函数
// 创建confdChannel, 绑定Httpdata事件
HttpData::HttpData(EventLoop *loop, int connfd):
    loop_(loop),
    connChannel_(std::make_shared<Channel>(loop, connfd)),
    connFd_(connfd),
    error_(false),
    nowReadPos_(0),
    keepAlive_(false),
    method_(METHOD_GET),            //GET
    HTTPVersion_(HTTP_11),          //1.1
    state_(STATE_PARSE_URI),
    hState_(H_START),
    connectionState_(H_CONNECTED) { 
    //设置connfdChannel的事件处理函数  
    connChannel_->setReadHandler([this](){
        this->handleRead(); 
    });
    connChannel_->setWriteHandler([this]{
        this->handleWrite();
    });
    connChannel_->setConnHandler([this](){
        this->handleConn();
    });
}

//重置
void HttpData::reset() {
    fileName_.clear();
    path_.clear();
    nowReadPos_ = 0;
    state_= STATE_PARSE_URI;
    hState_ = H_START;
    headers_.clear();
    if(timer_.lock()) {
        SP_TimerNode my_timer(timer_.lock());
        my_timer->clear();
        timer_.reset();
    }
}

//分离定时器
void HttpData::seperateTimer() {
    if(timer_.lock()) {
        SP_TimerNode my_timer(timer_.lock());
        my_timer->clear();
        timer_.reset();
    }
}

// 在定时器析构的时候调用
void HttpData::handleClose() {
    connectionState_ = H_DISCONNECTED; //断开连接
    //SP_HttpData guard(shared_from_this());
    loop_->removeFromPoller(connChannel_);//文件描述符最后由~HttpData关闭
}

// 将connfd对应的channel注册到epoll中EPOLLIN
void HttpData::newEvent() {     //子线程执行
    connChannel_->setEvents(DEFALT_EVENT);  //默认事件oneshot
    loop_->addToPoller(connChannel_, DEFAULT_EXPIRED_TIME); //定时器，两秒
}


// 处理通信描述符的读事件
void HttpData::handleRead() {
    //监听的事件
    uint32_t &events_ = connChannel_->getEvents(); //获得注册监听的事件
    do {
        bool isClosed = false;
        int read_num = readn(connFd_, inBuffer_, isClosed);
        //LOG << "\nRequest: \n" << inBuffer_;    //打印请求
        if(connectionState_ == H_DISCONNECTING) {
            inBuffer_.clear();
            break;
        }

        if(read_num < 0) {  //会小于0？
            perror("handleRead: 1");
            error_ = true;
            handleError(connFd_, 400, "Bad Request");
            break;
        } else if(isClosed) {
            connectionState_ = H_DISCONNECTING;
            if(read_num == 0) {
                break;
            }
        }
        //正常继续往下走
        if(state_ == STATE_PARSE_URI) { //请求行
            URIState flag = this->parseURI();
            if(flag == PARSE_URI_AGAIN) {
                break;
            } else if(flag == PARSE_URI_ERROR) {
                perror("HTTP Request Headers Error: ");
                inBuffer_.clear();
                error_ = true;
                handleError(connFd_, 400, "Bad Request");
                break;
            } else {
                state_ = STATE_PARSE_HEADERS;
            }
        }
        
        if(state_ == STATE_PARSE_HEADERS) {
            HeaderState flag = this->parseHeaders();
            if(flag == PARSE_HEADER_AGAIN) {
                break;
            } else if(flag == PARSE_HEADER_ERROR) {
                perror("handleRead: 3");
                error_ = true;
                handleError(connFd_, 400, "Bad Request");
                break;
            } 
            if(method_ == METHOD_POST) {
                state_ = STATE_RECV_BODY;
            } else {
                state_ = STATE_ANALYSIS;
            }
        }
        
        if(state_ == STATE_RECV_BODY) {
            int content_length = -1;
            if(headers_.find("Content-length") != headers_.end()) {
                content_length = stoi(headers_["Content-length"]);
            } else {
                error_ = true;
                handleError(connFd_, 400, "Bad Request: lost of Content-length");
                break;
            }
            if(static_cast<int>(inBuffer_.size()) < content_length) 
                break;
            state_ = STATE_ANALYSIS;
        }

        if(state_ == STATE_ANALYSIS) {
            AnalysisState flag = this->analysisRequest();
            if(flag == ANALYSIS_SUCCESS) {
                state_ = STATE_FINISH;
                break;
            } else {
                error_ = true;
                break;
            }
        }
    } while(false);

    if(!error_) {   //正确
        if(outBuffer_.size() > 0) {
            handleWrite();
        } 
        if(!error_ && state_ == STATE_FINISH) {
            this->reset();
            if(inBuffer_.size() > 0) {
                if(connectionState_ != H_DISCONNECTING) {
                    handleRead();
                }
            }
        } else if(!error_ && connectionState_ != H_DISCONNECTED) {
            events_ |= EPOLLIN;
        }
    }
}

// 处理通信描述符的写事件
void HttpData::handleWrite() {
    if(!error_ && connectionState_ != H_DISCONNECTED) {
        uint32_t &events_ = connChannel_->getEvents();
        if(writen(connFd_, outBuffer_) < 0) {
            perror("handleWrite: writen");
            events_ = 0;
            error_ = true;
        }
        if(outBuffer_.size() > 0) 
            events_ |= EPOLLOUT;
    }
}

// 处理通信描述符的连接事件
void HttpData::handleConn() {
    seperateTimer();
    uint32_t &events_ = connChannel_->getEvents();
    if(!error_ && connectionState_ == H_CONNECTED) {
        if(events_ != 0) {
            int timeout = DEFAULT_EXPIRED_TIME;
            if(keepAlive_) 
                timeout = DEFAULT_KEEP_ALIVE_TIME;
            if((events_ & EPOLLIN) && (events_ & EPOLLOUT)) {
                events_ = uint32_t(0);
                events_ |= EPOLLOUT;
            }
            events_ |= EPOLLET;
            loop_->updatePoller(connChannel_, timeout);
        } else if(keepAlive_) {
            events_ |= (EPOLLIN | EPOLLET);
            int timeout = DEFAULT_KEEP_ALIVE_TIME;
            loop_->updatePoller(connChannel_, timeout);
        } else {
            events_ |= (EPOLLIN | EPOLLET);
            int timeout = (DEFAULT_KEEP_ALIVE_TIME >> 1);
            loop_->updatePoller(connChannel_, timeout);
        }
    } else if(!error_ && connectionState_ == H_DISCONNECTING && (events_ & EPOLLOUT)) {
        events_ = (EPOLLOUT | EPOLLET);
    } else {
        loop_->runInLoop(bind(&HttpData::handleClose, shared_from_this()));
    }
}

// 处理通信描述符的连接事件
void HttpData::handleError(int fd, int err_num, std::string short_msg) {
    short_msg = " " + short_msg;
    char send_buff[4 * 1024];
    std::string body_buff, header_buff;
    body_buff += "<html><title>QAQ ERROR!</title>";
    body_buff += "<body bgcolor=\"ffffff\">";
    body_buff += std::to_string(err_num) + short_msg;
    body_buff += "<hr><em> Fangyx's Web </em>\n</body></html>";

    header_buff += "HTTP/1.1 " + std::to_string(err_num) + short_msg + "\r\n";
    header_buff += "Content-Type: text/html\r\n";
    header_buff += "Connection: Close\r\n";
    header_buff += "Content-Length: " + std::to_string(body_buff.size()) + "\r\n";
    header_buff += "\r\n";

    sprintf(send_buff, "%s", header_buff.c_str());
    writen(fd, send_buff, strlen(send_buff));
    sprintf(send_buff, "%s", body_buff.c_str());
    writen(fd, send_buff, strlen(send_buff));
}


//Http报文解析
URIState HttpData::parseURI() { //解析请求头
    std::string &str = inBuffer_;
    //std::string cop = str;
    size_t pos = str.find('\r', nowReadPos_);
    if(pos < 0) {
        return PARSE_URI_AGAIN;
    }
    std::string request_line = str.substr(0, pos);
    if(str.size() > pos + 1) 
        str = str.substr(pos + 1);
    else 
        str.clear();

    int posGet = request_line.find("GET");
    int posPost = request_line.find("POST");
    int posHead = request_line.find("HEAD");

    if(posGet >= 0) {
        pos = posGet;
        method_ = METHOD_GET;
    } else if(posPost >= 0) {
        pos = posPost;
        method_ = METHOD_POST;
    } else if(posHead >= 0) {
        pos = posHead;
        method_ = METHOD_HEAD;
    } else {
        return PARSE_URI_ERROR;
    }

    pos = request_line.find("/", pos);
    if(pos < 0) {
        fileName_ = "index.html";
        HTTPVersion_ = HTTP_11;
        return PARSE_URI_SUCCESS;
    } else {
        size_t _pos = request_line.find(' ', pos);
        if(_pos < 0) {
            return PARSE_URI_ERROR;
        } else {
            if(_pos - pos > 1) {
                fileName_ = request_line.substr(pos + 1, _pos - pos - 1);
                size_t __pos = fileName_.find("?");
                if(__pos >= 0) {
                    fileName_ = fileName_.substr(0, __pos);
                }
            } else {
                fileName_ = "index.html";
            }
        }
        pos = _pos;
    }
    
    pos = request_line.find("/", pos);
    if(pos < 0) {
        return PARSE_URI_ERROR;
    } else {
        if(request_line.size() - pos <= 3) {
            return PARSE_URI_ERROR;
        } else {
            std::string version = request_line.substr(pos + 1, 3);
            if(version == "1.0") {
                HTTPVersion_ = HTTP_10;
            } else if(version == "1.1") {
                HTTPVersion_ = HTTP_11;
            } else {
                return PARSE_URI_ERROR;
            }
        }
    }
    return PARSE_URI_SUCCESS;
}

HeaderState HttpData::parseHeaders() {  //解析请求行
    std::string &str = inBuffer_;
    int key_start = -1, key_end = -1, value_start = -1, value_end = -1;
    int now_read_line_begin = 0;
    bool notFinish = true;
    size_t i = 0;
    for(; i < str.size() && notFinish; i++) {
        switch(hState_) {
            case H_START:{
                if(str[i] == '\n' || str[i] == '\r') 
                    break;
                hState_ = H_KEY;
                key_start = i;
                now_read_line_begin = i;
                break;
            }
            case H_KEY: {
                if(str[i] == ':') {
                    key_end = i;
                    if(key_end - key_start <= 0) {
                        return PARSE_HEADER_ERROR;
                    }
                    hState_ = H_COLON;
                } else if(str[i] == '\n' || str[i] == '\r') {
                    return PARSE_HEADER_ERROR;
                }
                break;
            }
            case H_COLON: {
                if(str[i] == ' ') {
                    hState_ = H_SPACES_AFTER_COLON;
                } else {
                    return PARSE_HEADER_ERROR;
                }
                break;
            }
            case H_SPACES_AFTER_COLON: {
                hState_ = H_VALUE;
                value_start = i;
                break;
            }
            case H_VALUE: {
                if(str[i] == '\r') {
                    hState_ = H_CR;
                    value_end = i;
                    if(value_end - value_start <= 0) {
                        return PARSE_HEADER_ERROR;
                    }
                } else if(i - value_start > 255) {
                    return PARSE_HEADER_ERROR;
                }
                break;
            }
            case H_CR: {
                if(str[i] == '\n') {
                    hState_ = H_LF;
                    std::string key(str.begin() + key_start, str.begin() + key_end);
                    std::string value(str.begin() + value_start, str.begin() + value_end);
                    headers_[key] = value;
                    now_read_line_begin = i;
                } else {
                    return PARSE_HEADER_ERROR;
                }
                break;
            }
            case H_LF: {
                if(str[i] == '\r') {
                    hState_ = H_END_CR;
                } else {
                    key_start = i;
                    hState_ = H_KEY;
                }
                break;
            }
            case H_END_CR: {
                if(str[i] == '\n') {
                    hState_ = H_END_LF;
                } else {
                    return PARSE_HEADER_ERROR;
                }
                break;
            }
            case H_END_LF: {
                notFinish = false;
                key_start = i;
                now_read_line_begin = i;
                break;
            }
        }
    }
    if(hState_ == H_END_LF) {
        str = str.substr(i);
        return PARSE_HEADER_SUCCESS;
    }
    str = str.substr(now_read_line_begin);
    return PARSE_HEADER_AGAIN;
}

AnalysisState HttpData::analysisRequest() { //分析请求
    if(method_ == METHOD_POST) {

    } else if(method_ == METHOD_GET || method_ == METHOD_HEAD) {
        std::string header;
        header += "HTTP/1.1 200 OK\r\n";
        if(headers_.find("Connection") != headers_.end()
          && (headers_["Connection"] == "Keep-Alive" || 
              headers_["Connection"] == "Close")) {
                keepAlive_ = true;
                header += std::string("Connection: Keep-Alive\r\n");
                header += "Keep-Alive: timeout=" + std::to_string(DEFAULT_KEEP_ALIVE_TIME) + "\r\n";
              }
        int dot_pos = fileName_.find(".");
        std::string filetype;
        if(dot_pos < 0) {
            filetype = MimeType::getMime("default");
        } else {
            filetype = MimeType::getMime(fileName_.substr(dot_pos));
        }
       
        // test File 
        if(fileName_ == "hello") {
            outBuffer_ = "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\n\r\nHello World";
            return ANALYSIS_SUCCESS;
        }
        if(fileName_ == "favicon.ico") {
            header += "Content-type: image/png\r\n";
            header += "Content-length: " + std::to_string(sizeof(favicon)) + "\r\n";
            //header += "Server: Fangyx Web";

            header += "\r\n";
            outBuffer_ += header;
            outBuffer_ += std::string(favicon, favicon + sizeof(favicon));
            return ANALYSIS_SUCCESS;
        }
        // end of test 
        
            
        struct stat sbuf;
        if(stat(fileName_.c_str(), &sbuf) < 0) {
            header.clear();
            handleError(connFd_, 404, "NOT FOUND!");
            return ANALYSIS_ERROR;
        }
        header += "Content-type: " + filetype + "\r\n";
        header += "Content-length: " + std::to_string(sbuf.st_size) + "\r\n";
        header += "Server: Fangyx Webi\r\n";
        header += "\r\n";

        outBuffer_ += header;
        if(method_ == METHOD_HEAD) {
            return ANALYSIS_SUCCESS;
        }

        int src_fd = open(fileName_.c_str(), O_RDONLY, 0);
        if(src_fd < 0) {
            outBuffer_.clear();
            handleError(connFd_, 404, "NOT FOUND!");
            return ANALYSIS_ERROR;
        }
        void *mmapRet = mmap(nullptr, sbuf.st_size, PROT_READ, MAP_PRIVATE, src_fd, 0);
        close(src_fd);
        if(mmapRet == (void *)(-1)) {
            munmap(mmapRet, sbuf.st_size);
            outBuffer_.clear();
            handleError(connFd_, 404, "NOT FOUND!");
            return ANALYSIS_ERROR;
        }
        char *src_addr = static_cast<char *>(mmapRet);
        outBuffer_ += std::string(src_addr, src_addr + sbuf.st_size);
        munmap(mmapRet, sbuf.st_size);
        return ANALYSIS_SUCCESS;
    }
    return ANALYSIS_ERROR;
}
