#!/bin/zsh

# 12w打开文件上限
ulimit -n 12000

# 无限制coredump文件大小
ulimit -c unlimited

# 支持长连接 9w

#./WebBench/webbench -c 100 -t 6 -2 -k --get http://127.0.0.1:8000/index.html

# ./WebBench/webbench -c 1000 -t 60 -2 -k --get http://127.0.0.1:8000/hello.html

# 短链接 5w
./WebBench/webbench -c 100 -t 10 -2 --get http://127.0.0.1:8000/he

#./WebBench/webbench -c 100 -t 6 -2 --get http://127.0.0.1:8000/