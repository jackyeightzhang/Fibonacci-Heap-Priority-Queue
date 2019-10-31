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
typedef unordered_map<Node, pair<Node, Info>> Graph;

void printGraph(const Graph & g){
	cout << "%%%%%%%%%%%%%%%%%%%%%Printing Graph%%%%%%%%%%%%%%%%%%%%%%" << endl;
	for(auto it: g){
		cout << it.first << "->" << it.second.first << "(" << it.second.second << ")" << endl;
	}
	cout << "%%%%%%%%%%%%%%%%%%%%%Printing Complete%%%%%%%%%%%%%%%%%%%%%%" << endl;
}

void printNodeMap(const unordered_map<Node, set<Node>> & n){
	cout << "%%%%%%%%%%%%%%%%%%%%%Printing Node Map%%%%%%%%%%%%%%%%%%%%%%" << endl;
	for(auto it: n){
		cout << it.first << "---";
		for(auto el : it.second) cout << el << ",";
		cout << endl;
	}
	cout << "%%%%%%%%%%%%%%%%%%%%%Printing Complete%%%%%%%%%%%%%%%%%%%%%%" << endl;

}

void printInfoMap(const unordered_map<Node, Info> & im){
	cout << "%%%%%%%%%%%%%%%%%%%%%Printing Info Map%%%%%%%%%%%%%%%%%%%%%%" << endl;
	for(auto it : im)
		cout << it.first << "'s distance: " << it.second << endl;
	cout << "%%%%%%%%%%%%%%%%%%%%%Printing Complete%%%%%%%%%%%%%%%%%%%%%%" << endl;

}

void printInfoPQ(const vector<pair<Node, Info>> & pq){
	cout << "%%%%%%%%%%%%%%%%%%%%%Printing Priority Queue%%%%%%%%%%%%%%%%%%%%%%" << endl;
	for(auto it : pq)
		cout << it.first << "'s distance: " << it.second << endl;
	cout << "%%%%%%%%%%%%%%%%%%%%%Printing Complete%%%%%%%%%%%%%%%%%%%%%%" << endl;

}

void printAnswerMap(const unordered_map<Node, Info> & am){
	cout << "%%%%%%%%%%%%%%%%%%%%%Printing Priority Queue%%%%%%%%%%%%%%%%%%%%%%" << endl;
	for(auto it : am) cout << it.first << "'s distance: " << it.second << endl;
	cout << "%%%%%%%%%%%%%%%%%%%%%Printing Complete%%%%%%%%%%%%%%%%%%%%%%" << endl;

}
unordered_map<Node, Info>  extendedDijkstra( Node source, Graph graph ){
	unordered_map<Node, set<Node>> node_map;
	//PROBLEM
	//we cant have pairs as key's for unordered maps	
	unordered_map<pair<Node, Node>, Info> cost_map;
	for(auto it : graph) { 			//initialize node_map with all the nodes and their reachable edges
		if (node_map.find(it.first) == node_map.end()) node_map[it.first] = set<Node>();
		node_map[it.first].insert(it.second.first);
		//PROBLEM-Resolved: we are going to use unordered_maps instead b/c hash pairs might be easier for them
		if(cost_map.find(it.first) == cost_map.end()) cost_map[it.first] = make_pair(it.second.first,it.second.second);
		//i see problems here as cost_map requires me to compare its key which is a pair with another pair
		//	I do not have another pair that a can compare, and creating one might not work because of pointers
	}	
	printNodeMap(node_map);
//	printGraph(cost_map);
	unordered_map<Node, Info> answer_map;
	unordered_map<Node, Info> info_map;
	for(auto it : node_map){		//initialize info_map with all nodes and with Info to infinity
		info_map[it.first] = LONG_MAX;
	}
	info_map[source] = 0;		//set source node distance to 0
	
	vector<pair<Node, Info>> info_pq;
	for(auto it: info_map){
		info_pq.push_back(make_pair(it.first,it.second));
	}
	make_heap (info_pq.begin(), info_pq.end(), [](pair<Node, Info> a, pair<Node, Info> b){ return a.second > b.second;}); 
	//because the only way to pop is to to remove the last element, i need to reverse the priority  
	//printInfoMap(info_map);
	//printInfoPQ(info_pq);	
	
	while(!info_map.empty()){
		pair<Node, Info> min = info_pq[info_pq.size()-1]; //pop back does not return the value removed
		info_pq.pop_back();
		if(min.second == LONG_MAX) break;	
		info_map.erase(min.first);
		answer_map[min.first] = min.second;

		for (auto d : node_map[min.first]){
			if(answer_map.find(d) != answer_map.end()) continue;
			if(cost_map[pair<Node, Node>(min.first, d)] + min.second < info_map[d]){
				info_map[d] = cost_map[pair<Node, Node>(min.first, d)] + min.second;
//			        info_pq.
				make_heap (info_pq.begin(), info_pq.end(), [](pair<Node, Info> a, pair<Node, Info> b){ return a.second > b.second;}); 	
			}
		}
	}
	return answer_map;
}	

int main(){
	Graph g;
	for(int i = 0; i < 26; ++i){	
		g['a'+i] = pair<Node, Info>(rand() % 26+'a',rand() % 26);
		//PROBLEM - Resolved: we are going to rely on cost_map and node_map instead of extracting from Graph
		//// there is a problem with the graph implementation because i cant have multiple keys	
	}
	//printGraph(g);
	Node start = 'a';
	printAnswerMap(extendedDijkstra(start, g));
	return 0;
}
