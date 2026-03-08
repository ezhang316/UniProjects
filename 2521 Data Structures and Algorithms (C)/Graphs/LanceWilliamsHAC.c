// Lance-Williams Algorithm for Hierarchical Agglomerative Clustering

#include <assert.h>
#include <float.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


float Lance_Williams(int method, float Dki, float Dkj);
Dendrogram new_dendrogram_node(int v, Dendrogram r, Dendrogram l);

/**
 * Generates  a Dendrogram using the Lance-Williams algorithm (discussed
 * in the spec) for the given graph  g  and  the  specified  method  for
 * agglomerative  clustering. The method can be either SINGLE_LINKAGE or
 * COMPLETE_LINKAGE (you only need to implement these two methods).
 * 
 * The function returns a 'Dendrogram' structure.
 */
Dendrogram LanceWilliamsHAC(Graph g, int method) {

    // Getting number of vertices
    int v = GraphNumVertices(g);

    double d[v][v];
    Dendrogram *dendrograms = malloc(sizeof(Dendrogram) * v);
    
    // Initialise dendrograms with vertices and distance array with DBL_MAX
    for (int i = 0; i < v; i++) {
        dendrograms[i] = new_dendrogram_node(i, NULL, NULL);
        for (int j = 0; j < v; j++) {
            d[i][j] = 0;
        }
    }
    
    // Calculating real edge distances into distance array
    for (int i = 0; i < v; i++) {
        AdjList out = GraphOutIncident(g, i);
        while (out != NULL) {
            d[i][out->v] = out->weight;
            out = out->next;
        }
    }
    
    // Turning directed edge distances into 1/(max distance between nodes)
    for (int i = 0; i < v; i++) {
        for (int j = i + 1; j < v; j++) {
            // If two vertices are not connected
            if (d[j][i] == 0 && d[i][j] == 0) {
                d[i][j] = DBL_MAX;
                d[j][i] = DBL_MAX;
            }
            // If two vertices are connected by one edge, take that edge
            else if (d[j][i] == 0) {
                d[i][j] = 1/d[i][j];
                d[j][i] = d[i][j];
            }
            // If two vertices are connected by one edge, take that edge
            else if (d[i][j] == 0) {
                d[i][j] = 1/d[j][i];
                d[j][i] = d[i][j];
            }
            // If two vertices are connected by more than one edge, take the largest
            else {
                d[i][j] = 1/Lance_Williams(COMPLETE_LINKAGE, d[i][j], d[j][i]);
                d[j][i] = d[i][j];
            }
        }
    }
    
    // Merge nV - 1 times
    for (int i = 0; i < v - 1; i++) {
        // Find smallest distance in distance array + associated clusters
        int cluster_1;
        int cluster_2;
        double value = DBL_MAX;
        for (int j = 0; j < v - i; j++) {
            for (int k = j + 1; k < v - i; k++) {
                if (d[j][k] < value) {
                    value = d[j][k];
                    cluster_1 = j;
                    cluster_2 = k;
                }
            }
        }
        
        // Merging clusters and removing them from dendA
        Dendrogram new = new_dendrogram_node(-1, dendrograms[cluster_1], dendrograms[cluster_2]);
        dendrograms[cluster_1] = NULL;
        dendrograms[cluster_2] = NULL;

        // Shifting all clusters left and putting new cluster at end
        for (int l = 0; l < v - i; l++) {
            // If current spot empty, get next dendrogram in spot/spot after next and shift to current
            // Also makes sure nothing executes if spot after next is out of bounds
            if (dendrograms[l] == NULL && l + 2 < v - i) {
                if (dendrograms[l + 1] == NULL) {
                    dendrograms[l] = dendrograms[l + 2];
                    dendrograms[l + 2] = NULL;
                }
                else {
                    dendrograms[l] = dendrograms[l + 1];
                    dendrograms[l + 1] = NULL;
                }
            }
            // Inserts new dendrogram at end of non-empty spots
            if (l == (v - i - 2)) {
                dendrograms[l] = new;
            }
        }
        
        // Calculating distances from new cluster to other clusters and storing in tmp
        double tmp[v];
        int count = 0;
        for (int j = 0; j < v - i; j++) {
            if (j != cluster_1 && j != cluster_2) {
                tmp[count] = Lance_Williams(method, d[cluster_1][j], d[cluster_2][j]);
                count++;
            }
        }
        // Moving unchanged distances in distance array filling in the gap where merged clusters used to be
        int gap_filler_row = 0;
        for (int j = 0; j < v - i; j++) {
            // If current is a merged cluster, adds to row offset
            if (j == cluster_1 || j == cluster_2) {
                gap_filler_row++;
            }
            else {
                int gap_filler_col = 0;
                for (int k = j + 1; k < v - i; k++) {
                    // If current is a merged cluster, adds to column offset
                    if (k == cluster_1 || k == cluster_2) {
                        gap_filler_col++;
                    }
                    // Otherwise moves current to new position
                    else {
                        d[j - gap_filler_row][k - gap_filler_col] = d[j][k];
                        d[k - gap_filler_col][j - gap_filler_row] = d[j][k];
                    }
                }
            }
        }
        // Putting merged cluster distances into distance array at end of non-empty rows/cols
        for (int j = 0; j < v - i; j++) {
            d[j][v - 2 - i] = tmp[j];
            d[v - 2 - i][j] = tmp[j];
        }
    }
    Dendrogram ptr = dendrograms[0];
    free(dendrograms);
    return ptr;
}


Dendrogram new_dendrogram_node(int v, Dendrogram r, Dendrogram l) {
    Dendrogram new = malloc(sizeof(DNode));
    new->vertex = v;
	new->right = r;
	new->left = l;
	return new;
}

float Lance_Williams(int method, float Dki, float Dkj) {

    if (Dki > DBL_MAX) {
        return Dkj;
    }
    if (Dkj > DBL_MAX) {
        return Dki;
    }
    // Calculating new distance based on method
    float update;
    if (method == SINGLE_LINKAGE) {
        update = 0.5 * (Dki + Dkj - fabs(Dki - Dkj));
    }
    else if (method == COMPLETE_LINKAGE) {
        update = 0.5 * (Dki + Dkj + fabs(Dki - Dkj));
    }
    return update;
}