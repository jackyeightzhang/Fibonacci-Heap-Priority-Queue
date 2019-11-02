CXX		:= g++
CXXFLAGS	:= -std=c++11 -ggdb

INC_PATH	:= -Iinclude/
LIB_PATH	:= -Llib/
LFLAGS		:= -lgtest_main -lgtest -lpthread

all: gtest sandbox

sandbox:
	$(CXX) $(CXXFLAGS) $(INC_PATH) src/Sandbox.cpp $(LIB_PATH) $(LFLAGS) -o bin/sandbox

gtest:
	$(CXX) $(CXXFLAGS) $(INC_PATH) src/UnitTest.cpp $(LIB_PATH) $(LFLAGS) -o bin/gtest

program:
	$(CXX) $(CXXFLAGS) $(INC_PATH) src/extendeddijkstra.cpp -o bin/program

run_program:
	./bin/program

run_sandbox:
	./bin/sandbox

run_gtest:
	./bin/gtest

.PHONY: clean

clean: 
	rm bin/*
