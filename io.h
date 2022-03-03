#include <bits/stdc++.h>
#include <mpi.h>

int **readGraph(std::string filename, int num_nodes, int num_edges, int* num_child, int rank, int num_proc);
int **readGraph_basic(std::string filename, int num_nodes, int num_edges, int* num_child, int rank, int num_proc);

void writeOutput(MPI_File fs, unsigned int outdegree, unsigned int *result, int num_rec, int num_found, int nodeid);
void writeOutput_basic(std::fstream& fs, unsigned int outdegree, unsigned int *result, int num_rec, int num_found, int nodeid);

void convertOutput(int num_nodes, int num_rec);