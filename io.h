#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <mpi.h>

int **readGraph(std::string filename, int num_nodes, int num_edges, int* num_child, int rank, int num_proc);

void writeOutput(std::fstream& fs, unsigned int outdegree, unsigned int *result, int num_rec, int num_found);