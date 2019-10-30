#include <vector>
#include <map>
#include <utility>
#include <set>
#include <iostream>
#include <limits.h>
using namespace std;

typedef char* Node;
typedef map<char*,pair<char*,long>> Graph;

map<Node, long>  extendedDijkstra( Node source, Graph graph ){
	map<Node, set<Node>> node_map;
	for(auto it : graph) { //initialize node_map with all the nodes and their reachable edges
		node_map[it.first].insert(it.second.first);
	}
	map<pair<Node,Node>, long> cost_map;
	map<Node, long> answer_map;
	map<Node, long> info_map;
	cout << "data structures initialized" << endl;
	for(auto it : node_map){		//initialize info_map with all nodes and with Info to infinity
		info_map[it.first] = LONG_MAX;
	}
	info_map[source] = 0;		//set source node distance to 0

	//info_pq : pair(Node, Info)
	//sorted by smallest distance 
	//
	//while info_map not empty:
	//current = info_pq.pop()
	//if(current.dist == INFINITY) STOP
	//for node_map[current.node]:
	//   min = find smallest distance
	//info_map.remove(min)
	//answer_map.insert(min)
	//
	//for d in node_map[min.node] :
	//    if d in answer_map: continue
	//    if cost_map[pair(min, d)] + min.dist < info_map[d]:
	//        info_map[d] = cost_map[pair(min, d)] + min.dist
	//        info_pq.percolate_up(pair(d, info_map[d]))
	return answer_map;
}	

int main(){
	Graph g;
	Node start;
	extendedDijkstra(start, g);
	return 0;
}


