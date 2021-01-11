// Dijkstra ADT implementation
// COMP2521 Assignment 2

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "Dijkstra.h"
#include "Graph.h"
#include "PQ.h"


ShortestPaths dijkstra(Graph g, Vertex src) {
	ShortestPaths *sp = malloc(sizeof(struct ShortestPaths));
	sp->numNodes = GraphNumVertices(g);
	sp->src = src;
	sp->dist = malloc(sp->numNodes * sizeof(int));
	sp->pred = malloc(sp->numNodes * sizeof(struct PredNode));
	// Initialize the dist array to infinity and pred array to -1
	for (int i = 0; i < sp->numNodes; i++) {
		sp->dist[i] = INT_MAX;
		sp->pred[i] = NULL;
	}
	PQ pq = PQNew();
	
	// Making all distance infinity in the PQ
	for (int i = 0; i < sp->numNodes; i++) {
		PQInsert(pq, i, INT_MAX);
	}
	// Reset distance from source to source to 0 to start
	PQUpdate(pq, src, 0);
	sp->dist[src] = 0;
	while (PQIsEmpty(pq) == false) {	
		// Dequeue the element with the smallest value(distance)
		Vertex v = PQDequeue(pq);		
		// If the smallest value is infinity, end the process as we know 
		// all the node left in PQqueue are unreachable, set them all to 0.
		if (sp->dist[v] == INT_MAX) {
			for (int i = 0; i < sp->numNodes; i++) {
				if (sp->dist[i] == INT_MAX) {
					sp->dist[i] = 0;
				}
			}	
			break;
		}
		AdjList curr = GraphOutIncident(g, v);
		while (curr != NULL) {									
			// New weight + previous distance = distance in array
			if ((sp->dist[v] + curr->weight) == sp->dist[curr->v]) {     
				// Creating new prednode
				PredNode *prednode = malloc(sizeof(struct PredNode));
				prednode->v = v;
				prednode->next = NULL;
				PredNode *p_curr = sp->pred[curr->v];
				PredNode *prev = NULL;
				// Go to the end of linked list 
				while (p_curr != NULL) {                    
					prev = p_curr;
					p_curr = p_curr->next;
				}                      
				prev->next = prednode;  // Insert another pred node 
			}
			// New weight + previous distance < distance in array
			else if ((sp->dist[v] + curr->weight) < sp->dist[curr->v]) { 
				sp->dist[curr->v] = sp->dist[v] + curr->weight;	         
				PQUpdate(pq, curr->v, curr->weight + sp->dist[v]);   
				// Creating new prednode
				PredNode *prednode = malloc(sizeof(struct PredNode));
				prednode->v = v;
				prednode->next = NULL;
				PredNode *p_curr = sp->pred[curr->v];
				PredNode *prev = NULL;
				if (p_curr != NULL) {
					// Go through pred to free every node to insert
					// new node with less distance.
					while (p_curr != NULL) {	
						prev = p_curr;
						p_curr = p_curr->next;
						free(prev);
					}								     
				}
				sp->pred[curr->v] = prednode;
			}									
			curr = curr->next;									
		}
	}
	PQFree(pq);
	return *sp;
}

void showShortestPaths(ShortestPaths sps) {

}
// Freeing memery malloced e.g linked list of 
// predeccsors and distant array.
void freeShortestPaths(ShortestPaths sps) {
	free(sps.dist);
	for (int i = 0; i < sps.numNodes; i++) {
		PredNode* curr = sps.pred[i];
		PredNode* prev = NULL;
		while (curr != NULL) {
			prev = curr;
			curr = curr->next;
			free(prev);
		}
	}
	free(sps.pred);
}

