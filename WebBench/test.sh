#!/bin/sh

# 支持长连接

./webbench -t 6 -c 100 -2 -k --get  http://127.0.0.1:8000/ine
