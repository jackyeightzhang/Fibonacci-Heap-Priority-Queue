#include <vector>
#include <unordered_map>
#include <utility>
#include <set>
#include <iostream>
#include <limits.h>
#include <algorithm>
#include <map>
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

void printInfoMap(const unordered_map<Node, Info> & pq){
	cout << "%%%%%%%%%%%%%%%%%%%%%Printing Priority Queue%%%%%%%%%%%%%%%%%%%%%%" << endl;
	for(auto it : pq)
		cout << it.first << "'s distance: " << it.second << endl;
	cout << "%%%%%%%%%%%%%%%%%%%%%Printing Complete%%%%%%%%%%%%%%%%%%%%%%" << endl;

}
unordered_map<Node, Info>  extendedDijkstra( Node source, Graph graph ){
	map<Node, set<Node>> node_map;
	map<pair<Node,Node>, Info> cost_map;
	for(auto it : graph) { 			//initialize node_map with all the nodes and their reachable edges
		if (node_map.find(it.first) == node_map.end()) node_map[it.first] = set<Node>();
		node_map[it.first].insert(it.second.first);
		//PROBLEM-Resolved: we are going to use unordered_maps instead b/c hash pairs might be easier for them
		//if(cost_map.find(it.first) == node_map.end()) cost_map[it.first] = 
		//i see problems here as cost_map requires me to compare its key which is a pair with another pair
		//	I do not have another pair that a can compare, and creating one might not work because of pointers
	}	
	printNodeMap(node_map);

	unordered_map<Node, Info> answer_map;
	unordered_map<Node, Info> info_map;
	for(auto it : node_map){		//initialize info_map with all nodes and with Info to infinity
		info_map[it.first] = LONG_MAX;
	}
	info_map[source] = 0;		//set source node distance to 0
	
	vector<pair<Node, Info>> info_pq;
	make_heap (info_pq.begin(), info_pq.end(), [](pair<Node, Info> a, pair<Node, Info> b){ return a.second < b.second;});
	printInfoMap(info_map);
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
		g['a'+i] = pair<Node, Info>(rand() % 26+'a',rand() % 26);
		//PROBLEM - Resolved: we are going to rely on cost_map and node_map instead of extracting from Graph
		//// there is a problem with the graph implementation because i cant have multiple keys	
	}
	printGraph(g);
	Node start;
	extendedDijkstra(start, g);
	return 0;
}
