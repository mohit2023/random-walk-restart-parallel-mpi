#include "randomWalk.h"

void randomWalk(int node, int **adj, int *num_child, int *count, int num_steps, Randomizer r, int user_node) {
    int u = node;
    count[u]++;
    while(num_steps>0) {
        int childCount = num_child[u];
        if(childCount>0) {
            int next_step = r.get_random_value(user_node);
            if(next_step<0) {
                u = node;
            } else {
                int next_index = next_step%childCount;
                u = adj[u][next_index];
            }
        } else {
            u = node;
        }
        count[u]++;
        num_steps--;
    }
}