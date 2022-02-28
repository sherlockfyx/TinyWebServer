CC=g++
CCFLAGS= -std=c++14 -g -O3 -Wall -D_PTHREADS # -finput-charset=UTF-8 -fexec-charset=UTF-8 #-static
CCLINK=-lpthread

#	Source and Objects 
MAIN_SOURCE:=main.cpp  
SOURCE:=$(wildcard *.cpp ./test/*.cpp ./base/*.cpp) 
override SOURCE:=$(filter-out $(MAIN_SOURCE), $(SOURCE)) 
OBJECTS:=$(patsubst %.cpp, %.o, $(SOURCE)) 

#	Target
MAIN_TARGET:=./bin/WebServer 
SUB_TARGET:=./bin/test_log 

#	rule for Target 
all: $(MAIN_TARGET)  

%.o:%.cpp 
	$(CC) $(CCFLAGS) -c -o $@ $<


$(MAIN_TARGET):$(OBJECTS) main.o
	$(CC) $(CCFLAGS) -o $@ $^ $(CCLINK) 

main.o: main.cpp 
	$(CC) $(CCFLAGS) -c -o $@ $^



.PHONY:
clean:
	rm $(OBJECTS) $(all) main.o *.log log/* bin/* core* test/*.o -rf
time:
	date
print:
	@echo 'MAIN_SOURCE = $(MAIN_SOURCE)'
	@echo 'SOURCE = $(SOURCE)'
	@echo 'OBJECTS = $(OBJECTS)'
	@echo 'all = $(all)'