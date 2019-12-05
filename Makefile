CXX			:= g++
CXXFLAGS	:= -std=c++11 -ggdb

INC_PATH	:= -Iinclude/
LIB_PATH	:= -Llib/
LFLAGS		:= -lcourselib -lgtest_main -lgtest -lpthread

all: dijkstra driver_pq driver_graph gtest

dijkstra:
	$(CXX) $(CXXFLAGS) $(INC_PATH) src/dijkstra.cpp $(LIB_PATH) $(LFLAGS) -o bin/dijkstra
driver_pq:
	$(CXX) $(CXXFLAGS) $(INC_PATH) src/driver_priority_queue.cpp $(LIB_PATH) $(LFLAGS) -o bin/driver_pq
driver_graph:
	$(CXX) $(CXXFLAGS) $(INC_PATH) src/driver_graph.cpp $(LIB_PATH) $(LFLAGS) -o bin/driver_graph
gtest:
	$(CXX) $(CXXFLAGS) $(INC_PATH) src/test_priority_queue.cpp $(LIB_PATH) $(LFLAGS) -o bin/gtest


run_dijkstra:
	./bin/dijkstra
run_driver_pq:
	./bin/driver_pq
run_driver_graph:
	./bin/driver_graph
run_gtest:
	./bin/gtest

.PHONY: clean

clean: 
	rm bin/*
