#include "Server.h"
#include "Util.h"
#include <functional>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

/*
1.只有一个事件循环线程池，由服务器创建unique_ptr

2.封装监听文件描述符acceptChannel, 设置非阻塞

*/

Server::Server(EventLoop *loop, int threadNum, int port):
    loop_(loop),
    threadNum_(threadNum),
    eventLoopThreadPool_(std::make_unique<EventLoopThreadPool>(loop_, threadNum)),
    started_(false),
    acceptChannel_(std::make_shared<Channel>(loop_)),
    port_(port),
    listenFd_(socketBindListen(port_)) {

    acceptChannel_->setFd(listenFd_);
    handle_for_sigpipe();
    if(setSocketNonBlocking(listenFd_) < 0) {
        perror("set socket nonblocking failed");
        abort();
    }
}

/*
1.服务器启动, 将线程池启动
2.设置acceptChannel对应事件处理函数, 注册到epoll, 设置flag
*/
void Server::start() {
    eventLoopThreadPool_->start();
    acceptChannel_->setEvents(EPOLLIN | EPOLLET);
    acceptChannel_->setReadHandler([this](){
        this->handNewConn();    //设置新连接到来处理事件；
    });
    acceptChannel_->setConnHandler([this](){
        this->handThisConn();
    });
    loop_->addToPoller(acceptChannel_, 0);  //不加定时器
    started_ = true;
}

//主线程 ，处理新连接
void Server::handNewConn() {    
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    socklen_t client_addr_len = sizeof(client_addr);
    int connfd = 0 ;
    while((connfd = accept(listenFd_, (struct sockaddr*)&client_addr, &client_addr_len)) > 0) {
        //subReactor
        EventLoop *loop = eventLoopThreadPool_->getNextLoop();
        std::cout<<"Connect: " << inet_ntoa(client_addr.sin_addr) << ":" << ntohs(client_addr.sin_port)<<std::endl;

        if(connfd >= MAXFDS) {
            close(connfd);
            continue;
        }
        if(setSocketNonBlocking(connfd) < 0) {
            std::cout<< "set Non Blocking failed"<<std::endl;
            return;
        }
        setSocketNodelay(connfd);

        
        //设置 创建HttpData任务, 里面封装connfdChannel
        SP_HttpData req_info = std::make_shared<HttpData>(loop, connfd);    //初始化
        req_info->getChannel()->setHolder(req_info);    //设置connChannel所属的Httpdata
        loop->queueInLoop([req_info](){
            req_info->newEvent();   //将新连接注册到subReactor的epoll中
        });
    }
    acceptChannel_->setEvents(EPOLLIN | EPOLLET);
}