#include <algorithm>
#include <functional>
#include <iostream>
#include <limits.h>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

using namespace std;

typedef char			Node;
typedef unsigned long		Info;
typedef pair<Node, Node>	Edge;

struct pairHasher {
	size_t operator()(const Edge& key) const {
		return hash<Node>()(key.first) ^ hash<Node>()(key.second);
	}
};

typedef unordered_set<Node>				NodeSet;
typedef unordered_map<Node, NodeSet>			NodeMap;
typedef pair<Node, Info>				InfoPair;
typedef unordered_map<Node, Info>			InfoMap;
typedef vector<InfoPair>				InfoPQ;
typedef unordered_map<Edge, Info, pairHasher>		CostMap;
typedef unordered_map<Node, int>			IndexMap;

bool heapComp(const InfoPair& a, const InfoPair& b) {
	return a.second < b.second;
}

void debugPrint(const NodeMap& nodeMap) {
	for(const auto& nodePair: nodeMap) {
		cout << nodePair.first << ": ";
		for(const auto& node : nodePair.second) {
			cout << node << ", ";
		}
		cout << endl;
	}
}

void debugPrint(const InfoMap& infoMap) {
	for(const auto& infoPair : infoMap) {
		cout << infoPair.first << ": " << infoPair.second << endl;
	}
}

void debugPrint(const InfoPQ& infoPQ) {
	for(const auto& infoPair : infoPQ) {
		cout << infoPair.first << ": " << infoPair.second << endl;
	}
}

void debugPrint(const CostMap& costMap) {
	for(const auto& costPair : costMap) {
		cout << costPair.first.first << "-" << costPair.first.second << ": " << costPair.second << endl;
	}
}

void percolateUp(InfoPQ& infoPQ, HeapMap& indexMap, Node node) {
	int current = indexMap[node];
	int parent = (it - 1) / 2;
	while(current && infoPQ[parent].second > infoPQ[current].second) {
		swap(infoPQ[current], infoPQ[parent]);
		swap(indexMap[infoPQ[current].first], indexMap[infoPQ[parent].first])
		current = parent;
		parent = (it - 1) / 2;
	}	
}

InfoMap extendedDijkstra(const Node& source, const NodeMap& nodeMap, const CostMap& costMap) {
	debugPrint(nodeMap);
	debugPrint(costMap);

	InfoMap distMap;
	InfoMap minDistMap;

	IndexMap indexMap;
	
	// Initialize distMap with all node distances set to Infinity
	for(const auto& nodePair : nodeMap) {
		distMap[nodePair.first] = ULONG_MAX;
	}
	// Set source node distance to 0
	distMap[source] = 0;

	// Initialize a min-heap for all values in distMap
	InfoPQ distPQ(distMap.begin(), distMap.end());
	// Heapify's vector
	make_heap(distPQ.begin(), distPQ.end(), heapComp); 
	
	for(int i = 0; i < distPQ.size(); ++i) {
		indexMap[distPQ[i].first] = i;
	}

	while(!distMap.empty()) {
		// Extract min value from distPQ
		InfoPair min = distPQ[0];
		pop_heap(distPQ.begin(), distPQ.end(), heapComp);
		distPQ.pop_back();

		// Terminate if min distance is Infinity
		if(min.second == ULONG_MAX) break;

		// Remove min from distMap, add to minDistMap
		distMap.erase(min.first);
		minDistMap[min.first] = min.second;

		for (const auto& adjacentNode : nodeMap.at(min.first)) {
			if(minDistMap.count(adjacentNode)) continue;
			if(costMap.at(Edge(min.first, adjacentNode)) + min.second < distMap[adjacentNode]) {
				distMap[adjacentNode] = costMap.at(Edge(min.first, adjacentNode)) + min.second;
				distPQ[indexMap[adjacentNode]] = costMap.at(Edge(min.first, adjacentNode)) + min.second;
				percolateUp(distPQ, indexMap, adjacentNode);
			}
		}
	}
	return minDistMap;
}	

int main() {
	// @TODO generate NodeMap and CostMap randomly

	/*for(int i = 0; i < 26; ++i) {
		g['a'+i] = pair<Node, Info>(rand() % 26+'a',rand() % 26);
	}*/

	
	Node start = 'a';
	//debugPrint(extendedDijkstra(start, ));
	return 0;
}
