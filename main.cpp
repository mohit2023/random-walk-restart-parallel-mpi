#include "io.h"
#include "randomWalk.h"
#include <assert.h>

using namespace std;

int main(int argc, char* argv[]){

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

    double start=0,end=0;
    std:: fstream fs;
    if(rank==0) {
        start = MPI_Wtime();
        fs.open("output.dat", std::ios::out | std::ios::binary);
    }

    int *num_child = new int[num_nodes];
    int **adj = readGraph(graph_file, num_nodes, num_edges, num_child, rank, size);
    // printf("read done in proc: %d \n", rank);

    for(int i=0;i<num_nodes;i++) {
        int total = num_walks*num_child[i];
        int per_bucket = total/size;
        int extra = total%size;
        int start = per_bucket*rank;
        start = start + (rank<extra ? rank : extra);
        int end = start + per_bucket;
        end = end + (rank<extra ? 1 : 0);
        // printf("before in proc: %d for node: %d \n", rank, i);

        int *count = new int[num_nodes]();
        for(int j=start;j<end;j++) {
            int node_index = j/num_walks;
            int node = adj[i][node_index];
            randomWalk(node, adj, num_child, count, num_steps, random_generator, i);
        }
        // printf("random walk done in proc: %d for node: %d \n", rank, i);

        if(rank!=0) {
            MPI_Send(count, num_nodes, MPI_INT, 0, i, MPI_COMM_WORLD);
        } else {
            for(int j=0;j<size;j++) {
                if(j==rank) {
                    continue;
                }
                int *tmp = new int[num_nodes];
                MPI_Recv (tmp, num_nodes, MPI_INT, j, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                for(int k=0;k<num_nodes;k++) {
                    count[k]+=tmp[k];
                }
            }
            for(int k=0;k<num_child[i];k++) {
                count[k]=0;
            }

            priority_queue<pair<int,int>> pq;
            for(int k=0;k<num_nodes;k++) {
                if(count[k]!=0) {
                    if(pq.size() < num_rec) {
                        pq.push({count[k],k});
                    }
                    else if(pq.top().first < count[k]) {
                        pq.pop();
                        pq.push({count[k],k});
                    }
                }
            }

            int num_found = pq.size();
            unsigned int *result = new unsigned int[pq.size()*2];
            while(!pq.empty()) {
                std::pair<int,int> p = pq.top();
                pq.pop();
                result[pq.size()*2] = p.second;
                result[pq.size()*2+1] = p.first;
            }

            writeOutput(fs, num_child[i], result, num_rec, num_found);
            // cout<<i<<" : "<<num_child[i]<<"\n";
            // for(int i=0;i<num_found;i++){
            //     cout<<result[i*2]<<" : "<<result[i*2+1]<<"\n";
            // }
            // for(int i=num_found;i<num_rec;i++){
            //     cout<<"NULL : NULL\n";
            // }
        }
        // printf("end in proc: %d for node: %d \n", rank, i);
    }
    
    if (rank != 0) {
        int number = -1;
        MPI_Send(&number, 1, MPI_INT, 0, num_nodes, MPI_COMM_WORLD);
    } else {
        for(int i=1;i<size;i++) {
            int number;
            MPI_Recv(&number, 1, MPI_INT, i, num_nodes, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    if(rank==0){
        fs.close();
        end = MPI_Wtime();
        printf("Work took %f seconds\n", end - start);
        // convertOutput(num_nodes, num_rec);
    }
    // print_random(rank, num_nodes, random_generator);
    
    MPI_Finalize();
    return 0;
}