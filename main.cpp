#include "io.h"
#include <assert.h>
#include "randomizer.hpp"

using namespace std;

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

int main(int argc, char* argv[]){

    auto begin = std::chrono::high_resolution_clock::now();

    assert(argc > 8);
    std::string graph_file = argv[1];
    int num_nodes = std::stoi(argv[2]);
    int num_edges = std::stoi(argv[3]);
    float restart_prob = std::stof(argv[4]);
    int num_steps = std::stoi(argv[5]);
    int num_walks = std::stoi(argv[6]);
    int num_rec = std::stoi(argv[7]);
    int seed = std::stoi(argv[8]);
    
    //Only one randomizer object should be used per MPI rank, and all should have same seed
    Randomizer random_generator(seed, num_nodes, restart_prob);
    int rank, size;

    //Starting MPI pipeline
    MPI_Init(NULL, NULL);
    // Extracting Rank and Processor Count
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);


    int *num_child = new int[num_nodes]();
    int **adj = readGraph(graph_file, num_nodes, num_edges, num_child, rank, size);
    // printf("read done in proc: %d \n", rank);
    vector<pair<int,int>> sn(num_nodes);
    for(int i=0;i<num_nodes;i++){
        sn[i] = {num_child[i],i};
    }
    sort(sn.begin(),sn.end());
    int *count = new int[num_nodes];

    fstream fs("output.dat", std::ios::out | std::ios::binary);
    if(rank==0){
        fs.seekp(num_nodes*4*(2*num_rec+1));
    }
    MPI_Barrier(MPI_COMM_WORLD);
    
    for(int i=rank;i<num_nodes;i+=size) {
        int nodeid = sn[i].second;
        memset(count, 0, num_nodes*sizeof(int));
        for(int j=0;j<num_child[nodeid];j++) {
            for(int k=0;k<num_walks;k++){
                randomWalk(adj[nodeid][j], adj, num_child, count, num_steps, random_generator, nodeid);
            }
        }
        count[nodeid]=0;
        for(int k=0;k<num_child[nodeid];k++) {
            count[adj[nodeid][k]]=0;
        }
        
        priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
        for(int k=0;k<num_nodes;k++) {
            if(count[k]!=0) {
                if(pq.size() < num_rec) {
                    pq.push({count[k],-k});
                }
                else if(pq.top().first < count[k]) {
                    pq.pop();
                    pq.push({count[k],-k});
                }
            }
        }
        int num_found = pq.size();
        unsigned int *result = new unsigned int[pq.size()*2];
        while(!pq.empty()) {
            std::pair<int,int> p = pq.top();
            pq.pop();
            result[pq.size()*2] = -p.second;
            result[pq.size()*2+1] = p.first;
        }
        writeOutput_basic(fs, num_child[nodeid], result, num_rec, num_found, nodeid);
        delete[] result;
        // printf("end in proc: %d for node: %d \n", rank, i);
    }

    delete[] count;
    delete[] num_child;
    for(int i=0;i<num_nodes;i++) delete[] adj[i];
    delete[] adj;

    // print_random(rank, num_nodes, random_generator);
    // if (rank != 0) {
    //     int number = -1;
    //     MPI_Send(&number, 1, MPI_INT, 0, num_nodes, MPI_COMM_WORLD);
    // } else {
    //     for(int i=1;i<size;i++) {
    //         int number;
    //         MPI_Recv(&number, 1, MPI_INT, i, num_nodes, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //     }
    // }

    MPI_Barrier(MPI_COMM_WORLD);
    
    MPI_Finalize();
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    double duration = (1e-6 * (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin)).count());

    printf("Time taken by proc: %d is : %f \n", rank, duration);
    fs.close();

    return 0;
}
