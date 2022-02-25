#include "io.h"
#include "randomWalk.h"
#include <assert.h>

using namespace std;

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

    std:: fstream fs;
    if(rank==0) {
        fs.open("output.dat", std::ios::out | std::ios::binary);
    }

    int *num_child = new int[num_nodes]();
    int **adj = readGraph(graph_file, num_nodes, num_edges, num_child, rank, size);
    // printf("read done in proc: %d \n", rank);

    int *count = new int[num_nodes];
    int *count_global;
    if(rank==0) {
        count_global = new int[num_nodes];
    }
    for(int i=0;i<num_nodes;i++) {
        int total = num_walks*num_child[i];
        int per_bucket = total/size;
        int extra = total%size;
        int start = per_bucket*rank;
        start = start + (rank<extra ? rank : extra);
        int end = start + per_bucket;
        end = end + (rank<extra ? 1 : 0);
        // printf("before in proc: %d for node: %d \n", rank, i);

        memset(count, 0, num_nodes*sizeof(int));
        for(int j=start;j<end;j++) {
            int node_index = j/num_walks;
            int node = adj[i][node_index];
            randomWalk(node, adj, num_child, count, num_steps, random_generator, i);
        }
        // printf("random walk done in proc: %d for node: %d \n", rank, i);
    
        MPI_Reduce(count, count_global, num_nodes, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
        if(rank==0) {
            count_global[i]=0;
            for(int k=0;k<num_child[i];k++) {
                count_global[adj[i][k]]=0;
            }

            priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
            for(int k=0;k<num_nodes;k++) {
                if(count_global[k]!=0) {
                    if(pq.size() < num_rec) {
                        pq.push({count_global[k],k});
                    }
                    else if(pq.top().first < count_global[k]) {
                        pq.pop();
                        pq.push({count_global[k],k});
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
            cout<<i<<" : "<<num_child[i]<<"\n";
            for(int i=0;i<num_found;i++){
                cout<<result[i*2]<<" : "<<result[i*2+1]<<"\n";
            }
            for(int i=num_found;i<num_rec;i++){
                cout<<"NULL : NULL\n";
            }
            delete[] result;
        }
        // printf("end in proc: %d for node: %d \n", rank, i);
    }
    if(rank==0) delete[] count_global;
    delete[] count;
    delete[] num_child;
    for(int i=0;i<num_nodes;i++) delete[] adj[i];
    delete[] adj;

    if(rank==0){
        // convertOutput(num_nodes, num_rec);
        fs.close();
    }
    // print_random(rank, num_nodes, random_generator);
    
    MPI_Finalize();

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
    double duration = (1e-6 * (std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin)).count());

    printf("Time taken by proc: %d is : %f \n", rank, duration);

    return 0;
}
