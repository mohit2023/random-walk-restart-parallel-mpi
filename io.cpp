#include "io.h"

using namespace std;

int convertEndian(unsigned char* a, bool islittleendian) {
    if(islittleendian) {
        unsigned char tmp = a[0];
        a[0]=a[3];
        a[3]=tmp;
        tmp=a[1];
        a[1]=a[2];
        a[2]=tmp;
    }
    return *(unsigned int *)a;
}

int **readGraph(std::string filename, int num_nodes, int num_edges, int* num_child, int rank, int num_proc) {
    int num = 1;
    bool islittleendian = false;
    if (*(char *)&num == 1) {
        islittleendian = true;
    }

    std::fstream fs(filename, std::ios::in | std::ios::binary);
    int *data = new int[num_edges*2];
    for(int d=0; d<num_edges; d++) {
        unsigned char buffer[4];
        fs.read((char *)buffer, 4);
        data[d*2] = convertEndian(buffer, islittleendian);
        fs.read((char *)buffer, 4);
        data[d*2+1] = convertEndian(buffer, islittleendian);
    }
    fs.close();

    for(int d=0; d<num_edges; d++) {
        int u=data[d*2];
        num_child[data[d*2]]++;
    }

    int **adj = new int*[num_nodes];
    for(int i=0; i<num_nodes; i++) {
        adj[i] = new int[num_child[i]];
        num_child[i]=0;
    }

    for(int d=0; d<num_edges; d++) {
        int u=data[2*d];
        int v=data[2*d+1];
        adj[u][num_child[u]] = v;
        num_child[u]++;
    }

    return adj;
}

void writeOutput(std::fstream& fs, unsigned int outdegree, unsigned int *result, int num_rec, int num_found) {
    int num = 1;
    bool islittleendian = false;
    if (*(char *)&num == 1) {
        islittleendian = true;
    }

    unsigned char buffer[4];
    memcpy(buffer, (char*)&outdegree, sizeof(unsigned int));
    convertEndian(buffer, islittleendian);
    fs.write((char *)buffer, 4);
    for(int i=0;i<num_found;i++) {
        unsigned int node = result[i*2];
        memcpy(buffer, (char*)&outdegree, sizeof(unsigned int));
        convertEndian(buffer, islittleendian);
        fs.write((char *)buffer, 4);
        unsigned int infs = result[i*2+1];
        memcpy(buffer, (char*)&outdegree, sizeof(unsigned int));
        convertEndian(buffer, islittleendian);
        fs.write((char *)buffer, 4);
    }
    for(int i=num_found;i<num_rec;i++) {
        fs.write("NULLNULL", 8);
    }
}