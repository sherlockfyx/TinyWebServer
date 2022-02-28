#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include<arpa/inet.h>

#include "Util.h"

const int MAX_BUF = 4096;

ssize_t readn(int fd, void *buf, size_t n) {//一次读完
    size_t nleft = n;
    size_t nread = 0;
    size_t readSum = 0;
    char *ptr = (char *)buf;
    while(nleft > 0) {
        if((nread = read(fd, ptr, nleft)) < 0) {
            if(errno == EINTR) 
                nread = 0;
            else if (errno == EAGAIN) {
                return readSum;
            } else {
                return -1;
            }
        } else if(nread == 0) {
            break;
        }
        readSum += nread;
        nleft -= nread;
        ptr += nread;
    }
    return readSum;
}

ssize_t readn(int fd, std::string &inBuffer, bool& isClosed) {
    ssize_t nread = 0;
    ssize_t readSum = 0;
    while(true) {
        char buff[MAX_BUF];
        if((nread = read(fd, buff, MAX_BUF)) < 0) {
            // -1
            if(errno == EINTR) {    //被中断了
                continue;
            } else if(errno == EAGAIN) {    //读完了
                return readSum;
            } else {
                perror("read error");
                return -1;
            }
        } else if(nread == 0) { //对端关闭
            isClosed = true;
            break;
        }   
        readSum += nread;
        inBuffer += std::string(buff, buff + nread);
    }
    return readSum;
}

ssize_t readn(int fd, std::string &inBuffer) {
    ssize_t nread = 0;
    ssize_t readSum = 0;
    while(true) {
        char buff[MAX_BUF];
        if((nread = read(fd, buff, MAX_BUF)) < 0) {
            if(errno == EINTR) {
                nread = 0;
            } else if(errno == EAGAIN) {
                return readSum;
            } else {
                perror("read error");
                return -1;
            }
        } else if(nread == 0) {
            break;
        }
        readSum += nread;
        inBuffer += std::string(buff, buff + nread);
    }
    return readSum;
}

ssize_t writen(int fd, void *buff, size_t n) {
    ssize_t nleft = n;
    ssize_t nwritten = 0;
    ssize_t writeSum = 0;
    char *ptr = (char*)buff;
    while(nleft > 0) {
        if((nwritten = write(fd, ptr, nleft)) <= 0) {
            if(nwritten < 0) {
                if(errno == EINTR) {
                    nwritten = 0;
                } else if(errno == EAGAIN) {
                    return writeSum;
                } else {
                    return -1;
                }
            }
        }
        writeSum += nwritten;
        nleft -= nwritten;
        ptr += nwritten;
    }
    return writeSum;
}

ssize_t writen(int fd, std::string &sbuff) {
    size_t nleft = sbuff.size();
    ssize_t nwritten = 0;
    ssize_t writeSum = 0;
    const char *ptr = sbuff.c_str();
    while(nleft > 0) {
        if((nwritten = write(fd, ptr, nleft)) <= 0) {
            if(nwritten < 0) {
                if(errno == EINTR) {
                    nwritten = 0;
                } else if(errno == EAGAIN) {
                    break;
                } else {
                    return -1;
                }
            }
        }
        writeSum += nwritten;
        nleft -= nwritten;
        ptr += nwritten;
    }
    if(writeSum == static_cast<int>(sbuff.size())) 
        sbuff.clear();
    else 
        sbuff = sbuff.substr(writeSum);
    return writeSum;
}
//sigpipe信号--忽略
void handle_for_sigpipe() {
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    if(sigaction(SIGPIPE, &sa, nullptr)) 
        return ;
}

int setSocketNonBlocking(int fd) {
    int flag = fcntl(fd, F_GETFL, 0);
    if(flag == -1) {
        return -1;
    }
    flag |= O_NONBLOCK;
    if(fcntl(fd, F_SETFL, flag) < 0) {
        return -1;
    }
    return 0;
}

void setSocketNodelay(int fd) {
    int enable = 1;
    setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable));
}

void setSocketNoLinger(int fd) {
    struct linger linger_;
    linger_.l_onoff = 1;
    linger_.l_linger = 30;
    setsockopt(fd, SOL_SOCKET, SO_LINGER, (const char *)&linger_, sizeof(linger_));
}

void shurDownWR(int fd) {
    shutdown(fd, SHUT_WR);
}

int socket_bind_listen(int port) {

    int listen_fd = 0;
    if((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        return -1;
    }

    int optval = 1;//设置端口复用
    if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        return -1;
    }
    //服务器地址设置
    struct sockaddr_in server_addr;
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons((unsigned short)port);

    //socket与地址绑定
    if(bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        return -1;
    }
    //监听
    if(listen(listen_fd, 2048) == -1) {
        return -1;
    }

    if(listen_fd == -1) {
        close(listen_fd);
        return -1;
    }

    return listen_fd;
}
