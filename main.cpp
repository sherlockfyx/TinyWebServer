#include <iostream>
#include <thread>

#include "EventLoop.h"
#include "Server.h"


int main(int argc, char *argv[]) {
    //自动获取线程数
    int threadNum = 4;//std::thread::hardware_concurrency();
    int port = 8000;

    std::cout << "threadNum: " << threadNum << ", port: " << port << std::endl;

    EventLoop* mainReactor= new EventLoop();
    Server* HTTPServer = new Server(mainReactor, threadNum, port);
    
    HTTPServer->start();
    mainReactor->loop();

    delete mainReactor;
    delete HTTPServer;
    return 0;
}