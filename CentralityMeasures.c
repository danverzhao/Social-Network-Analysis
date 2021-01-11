// Centrality Measures ADT implementation
// COMP2521 Assignment 2
// By Danver Zhao
// zID: 5317086

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "CentralityMeasures.h"
#include "Dijkstra.h"
#include "PQ.h"

#define MAX_VERTEX 10000

void path_recur(PredNode** pred, int ba_for_recur[MAX_VERTEX], int s, int t, double* sum_paths);

// Closeness of a node is calculated as 1 over the sum of the length of the 
// shortest paths between the node (x) and all other nodes in the graph. 
// And for the assignment we need to normalise the result by timesing it
// by (n - 1)^2/(N - 1) where n is the number of nodes vertex u can reach 
// and N is the number of nodes in the Graph.
NodeValues closenessCentrality(Graph g) {
	NodeValues* closeness_values = malloc(sizeof(struct NodeValues));
	closeness_values->numNodes = GraphNumVertices(g);
	closeness_values->values = malloc(closeness_values->numNodes * sizeof(double));
	// We set u as the source node.
	for (int u = 0; u < closeness_values->numNodes; u++) {
		// Creating the dist and pred array with source vertex u.
		ShortestPaths ShortestPaths = dijkstra(g, u);
		double sum_of_dist = 0;
		double nodes_can_reach = 0;
		for (int v = 0; v < closeness_values->numNodes; v++) {
			sum_of_dist = sum_of_dist + ShortestPaths.dist[v];
			if (v != u && ShortestPaths.dist[v] != 0) {
				nodes_can_reach++;
			}
		}
		nodes_can_reach++;
		// Calculating final closeness ((n - 1)^2/(N - 1))*(1/Σ shortest paths to u).
		closeness_values->values[u] = ((nodes_can_reach - 1)/(closeness_values->numNodes - 1)) * 
			(((nodes_can_reach - 1)/sum_of_dist));
		// If the node is not connected to any other node, the closeness is 0.
		if (sum_of_dist == 0) {
			closeness_values->values[u] = 0;
		}
	}
	return *closeness_values;
}

// The betweenness centrality of a node v is given by the sum of (total number 
// of shortest paths from node s to node t divides number of those paths that 
// pass through v).
NodeValues betweennessCentrality(Graph g) {
	NodeValues* betweenness_values = malloc(sizeof(struct NodeValues));
	betweenness_values->numNodes = GraphNumVertices(g);
	betweenness_values->values = malloc(betweenness_values->numNodes * sizeof(double));
	// s is the source vertex.
	for (int s = 0; s < betweenness_values->numNodes; s++) {
		ShortestPaths s_paths = dijkstra(g, s);
		// t is the destination vertex.
		for (int t = 0; t < betweenness_values->numNodes; t++) {
			if (t != s && s_paths.dist[t] != 0) { 
				PredNode** pred = s_paths.pred;
				int ba_for_recur[MAX_VERTEX] = {0};
				double sum_paths_st = 0;
				// Count total number of paths from s to t and record what nodes 
				// are in between.
				path_recur(pred, ba_for_recur, s, t, &sum_paths_st);
				for (int v = 0; v < betweenness_values->numNodes; v++) {
					// If v appeared between s and t 
					if ( t != v && ba_for_recur[v] != 0) {
						double paths_s_v = 0;
						double paths_v_t = 0;
						// Finding number of path form t to v  and number of 
						// paths from v to s.
						// Because the number of paths through v from s to t
						// is the number of paths s-v times the number of path
						// v-t.
						pred = s_paths.pred;
						path_recur(pred, ba_for_recur, s, v, &paths_s_v);
						
						ShortestPaths s_paths_2 = dijkstra(g, v);
						pred = s_paths_2.pred;
						path_recur(pred, ba_for_recur, v, t, &paths_v_t);
						if (sum_paths_st != 0) {
							betweenness_values->values[v] += ((paths_s_v * paths_v_t)/(sum_paths_st));
						}
					}
				}
			}
		}
	}
	return *betweenness_values;
}

// Normalising the BetweennessCentrality by timesing 1/((n-1)*(n-2)), where n 
// is the number of nodes in the graph.
NodeValues betweennessCentralityNormalised(Graph g) {
	NodeValues n_betw_values;
	n_betw_values.numNodes = GraphNumVertices(g);
	n_betw_values.values = malloc(n_betw_values.numNodes * sizeof(double));
	NodeValues bc = betweennessCentrality(g);
	for (int i = 0; i < n_betw_values.numNodes; i++) {
		n_betw_values.values[i] = bc.values[i] * 
			1/((n_betw_values.numNodes - 1)*(n_betw_values.numNodes - 2));
	}
	return n_betw_values;
}
// Shows either normalised or just normal betweenness values for each vertex.
void showNodeValues(NodeValues nvs) {
	for (int i = 0; i < nvs.numNodes; i++) {
		printf("%d: %f\n", i, nvs.values[i]);
	}
}
// Freeing malloced memories.
void freeNodeValues(NodeValues nvs) {
	free(nvs.values);
}
// Counting the number of paths from s to t by back tracking from t, at the same 
// time marks what nodes are in between s and t in ba_for_recur.
void path_recur(PredNode** pred, int ba_for_recur[MAX_VERTEX], int s, int t, double* sum_paths) {
	// Back track from the pred array
	PredNode* prednode = pred[t];
	int k = prednode->v;
	// If t has multiple predecessors, then loop through it.
	while (prednode->next != NULL) {
		prednode = prednode->next;
		ba_for_recur[prednode->v]++;
		path_recur(pred, ba_for_recur, s, prednode->v, sum_paths);
	}
	// If t only has one predecessor vertex and it's the src.
	// Then number of paths from s to t + 1.
	if (k == s) {
		*sum_paths = *sum_paths + 1;
		return;
	}
	// If t has only 1 predecessor
	ba_for_recur[k]++;
	path_recur(pred, ba_for_recur, s, k, sum_paths);
}
