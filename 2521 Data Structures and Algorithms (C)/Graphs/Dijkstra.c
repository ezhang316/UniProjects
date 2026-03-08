// Dijkstra algorithm implementation

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

PredNode *Create_PredNode (Vertex ver);
void Free_PredNodeList (PredNode *pred);
void do_dijkstra(Graph g, NodeData *node_array, PQ pq);

NodeData *dijkstra(Graph g, Vertex src) {

    // Getting number of vertices
    int v = GraphNumVertices(g);
    if (v != 0) {
    
        // Mallocing node array
        NodeData *node_array = malloc(sizeof(NodeData) * v);
        
        // Initialising node array
        for (int i = 0; i < v; i++) {
            node_array[i].dist = INFINITY;
            node_array[i].pred = NULL;
        }
        
        // Setting distance from source node to itself to be zero
        node_array[src].dist = 0;
        
        // Setting up PQ and inserting only source node
        PQ pq = PQNew();
        PQInsert(pq, src, 0);
        
        // Recursive helper function to do the dijkstra algorithm
        do_dijkstra(g, node_array, pq);
        
        PQFree(pq);
        
	    return node_array;
    }
    else {
        return NULL;
    }
}

void freeNodeData(NodeData *data, int nV) {
    // First frees all pred node lists
    for (int i = 0; i < nV; i++) {
        Free_PredNodeList (data[i].pred);
    }
    // Frees given data array
    free(data);
}

// Helper function to do Dijkstra's algorithm
void do_dijkstra(Graph g, NodeData *node_array, PQ pq) {
    
    // Loops until PQ is empty
    while (!PQIsEmpty(pq)) {
        // Gets vertex from pq
        int src = PQDequeue(pq);
        // Getting vertex's outbound paths + weights
        AdjList out_list_head = GraphOutIncident(g, src);
        
        // Going through each outbound path starting from smallest vertex number
        while (out_list_head != NULL) {
            int current_distance = node_array[src].dist + out_list_head->weight;
            int vertex = out_list_head->v;
             
             
            // If current_distance is smaller than the old shortest distance, replaces
            // distance with current_distance
            if (node_array[vertex].dist > current_distance) {
            
                node_array[vertex].dist = current_distance;
                
                // If pred list isn't empty, frees entire list
                // and creates new predecessor using src
                if (node_array[vertex].pred != NULL) {
                    Free_PredNodeList(node_array[vertex].pred);
                    node_array[vertex].pred = Create_PredNode(src);
                }
                // If pred list is empty, creates new predecessor with src
                else {
                    node_array[vertex].pred = Create_PredNode(src);
                }
                // Inserting vertex into pq
                PQInsert(pq, vertex, current_distance);
            }
            // If current distance is equal, adds current node to pred list in ascending
            // order
            else if (node_array[vertex].dist == current_distance) {
            
                PredNode *current = node_array[vertex].pred;
                PredNode *previous = NULL;
                // Making previous the node before position to be inserted
                // and current the node after position to be inserted
                while (current->v < src) {
                    previous = current;
                    current = current->next;
                    if (current == NULL) {
                        break;
                    }
                }
                // If position to be inserted is in the middle, inserts pred node
                if (previous != NULL && current != NULL) {
                    PredNode *new = Create_PredNode(src);
                    new->next = current;
                    previous->next = new;
                }
                // If position to be inserted is at the end, inserts at end
                else if (previous!= NULL && current == NULL) {
                    PredNode *new = Create_PredNode(src);
                    previous->next = new;
                }
                // If position is first, inserts at front
                else {
                    PredNode *new = Create_PredNode(src);
                    new->next = current;
                    node_array[vertex].pred = new;
                }
            }
            
            // Next outbound path
            out_list_head = out_list_head->next;
        }
    }
}

// Helper function to create a pred node with given vertex
PredNode *Create_PredNode (Vertex ver) {
    PredNode *new_pred_node = malloc(sizeof(*new_pred_node));
    if (new_pred_node == NULL) {
        perror("unable to malloc new_pred_node");
        exit(1);
    }
    new_pred_node->v = ver;
    new_pred_node->next = NULL;
    return new_pred_node;
}
// Helper function to free list of prednodes given its head
void Free_PredNodeList (PredNode *pred) {
    PredNode *current = pred;
    PredNode *previous = NULL;
    
    while(current != NULL) {
        previous = current;
        current = current->next;
        free(previous);        
    }
}