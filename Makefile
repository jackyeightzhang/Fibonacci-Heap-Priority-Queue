CXX			:= g++
CXXFLAGS	:= -std=c++11 -ggdb

INC_PATH	:= -Iinclude/
LIB_PATH	:= -Llib/
LFLAGS		:= -lcourselib -lgtest_main -lgtest -lpthread

all: driver_pq  gtest

driver_pq:
	$(CXX) $(CXXFLAGS) $(INC_PATH) src/driver_priority_queue.cpp $(LIB_PATH) $(LFLAGS) -o bin/driver_pq
gtest:
	$(CXX) $(CXXFLAGS) $(INC_PATH) src/test_priority_queue.cpp $(LIB_PATH) $(LFLAGS) -o bin/gtest


run_driver_pq:
	./bin/driver_pq
run_gtest:
	./bin/gtest

.PHONY: clean

clean: 
	rm bin/*
