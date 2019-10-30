#include <vector>
#include <map>
#include <utility>
#include <set>
#include <iostream>
#include <limits.h>
#include <algorithm>
using namespace std;

typedef char Node;
typedef int Info;
typedef map<Node, pair<Node, Info>> Graph;

void printGraph(const Graph & g){
	cout << "%%%%%%%%%%%%%%%%%%%%%Printing Graph%%%%%%%%%%%%%%%%%%%%%%" << endl;
	for(auto it: g){
		cout << it.first << "->" << it.second.first << "(" << it.second.second << ")" << endl;
	}
	cout << "%%%%%%%%%%%%%%%%%%%%%Printing Complete%%%%%%%%%%%%%%%%%%%%%%" << endl;
}
void printCostMap(const map<pair<Node,Node>, Info> & c){}
void printNodeMap(const map<Node, set<Node>> & n){
	cout << "%%%%%%%%%%%%%%%%%%%%%Printing Node Map%%%%%%%%%%%%%%%%%%%%%%" << endl;
	for(auto it: n){
		cout << it.first << "---";
		for(auto el : it.second) cout << el << ",";
		cout << endl;
	}
	cout << "%%%%%%%%%%%%%%%%%%%%%Printing Complete%%%%%%%%%%%%%%%%%%%%%%" << endl;

}

map<Node, Info>  extendedDijkstra( Node source, Graph graph ){
	map<Node, set<Node>> node_map;
	map<pair<Node,Node>, Info> cost_map;
	for(auto it : graph) { 			//initialize node_map with all the nodes and their reachable edges
		if (node_map.find(it.first) == node_map.end()) node_map[it.first] = set<Node>();
		node_map[it.first].insert(it.second.first);
	}	
	printNodeMap(node_map);	
	map<Node, Info> answer_map;
	map<Node, Info> info_map;
	for(auto it : node_map){		//initialize info_map with all nodes and with Info to infinity
		info_map[it.first] = LONG_MAX;
	}
	info_map[source] = 0;		//set source node distance to 0
	
	vector<pair<Node, Info>> info_pq;
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
	for(int i = 0; i < 26; ++i){
		g['a'+i%26] = pair<Node, Info>(rand() % 26+'a',rand() % 26);// there is a problem with the graph implementation because i cant have multiple keys	
	}
	printGraph(g);
	Node start;
	extendedDijkstra(start, g);
	return 0;
}


