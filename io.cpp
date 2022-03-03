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

    MPI_File fh;
    MPI_File_open(MPI_COMM_WORLD, filename.c_str(), MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    unsigned char *buf = new unsigned char[4*num_edges*2];
    MPI_File_read_all(fh, buf, num_edges*2, MPI_INT, MPI_STATUS_IGNORE);
    MPI_File_close(&fh);
    int *data = new int[num_edges*2];
    for(int d=0; d<num_edges; d++) {
        unsigned char buffer[4];
        memcpy(buffer, buf+4*d*2, 4);
        data[d*2] = convertEndian(buffer, islittleendian);
        // if(rank==3) cout<<data[d*2]<<" ";
        memcpy(buffer, buf+4*(d*2+1), 4);
        data[d*2+1] = convertEndian(buffer, islittleendian);
        // if(rank==3) cout<<data[d*2+1]<<"\n";
    }
    delete[] buf;

    for(int d=0; d<num_edges; d++) {
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
    // if(rank==3) {
    //     for(int i=0;i<num_nodes;i++) {
    //         cout<<i<<" : ";
    //         for(int j=0;j<num_child[i];j++) {
    //             cout<<adj[i][j]<<" ";
    //         }
    //         cout<<"\n";
    //     }
    // }
    delete[] data;

    return adj;
}

int **readGraph_basic(std::string filename, int num_nodes, int num_edges, int* num_child, int rank, int num_proc) {
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
    delete[] data;

    return adj;
}

void copy(unsigned char buf[], unsigned char buffer[], int& x) {
    for(int i=0;i<4;i++){
        buf[x++] = buf[i];
    }
}

void writeOutput(MPI_File fs, unsigned int outdegree, unsigned int *result, int num_rec, int num_found, int nodeid) {
    int num = 1;
    bool islittleendian = false;
    if (*(char *)&num == 1) {
        islittleendian = true;
    }

    int per = 2*num_rec+1;
    unsigned char buf[per*4];
    int x = 0;
    unsigned char buffer[4];
    // if(nodeid==0) {
    //     outdegree = 175;
    //     char hex_string[20];
    //     sprintf(hex_string, "%X", outdegree); //convert number to hex
    //     cout<<hex_string<<"\n";
    // }
    memcpy(buffer, (char*)&outdegree, sizeof(unsigned int));
    convertEndian(buffer, islittleendian);
    // if(nodeid==0) {
    //     printf("%x %x %x %x\n", buffer[0], buffer[1], buffer[2], buffer[3]);
    // }
    copy(buf, buffer, x);
    // fs.write((char *)buffer, 4);
    for(int i=0;i<num_found;i++) {
        unsigned int node = result[i*2];
        memcpy(buffer, (char*)&node, sizeof(unsigned int));
        convertEndian(buffer, islittleendian);
        copy(buf, buffer, x);
        // fs.write((char *)buffer, 4);
        unsigned int infs = result[i*2+1];
        memcpy(buffer, (char*)&infs, sizeof(unsigned int));
        convertEndian(buffer, islittleendian);
        copy(buf, buffer, x);
        // fs.write((char *)buffer, 4);
    }
    buffer[0]='N';
    buffer[1]='U';
    buffer[2]='L';
    buffer[3]='L';
    for(int i=num_found;i<num_rec;i++) {
        copy(buf, buffer, x);
        copy(buf, buffer, x);
        // fs.write("NULLNULL", 8);
    }
    MPI_File_write_at(fs, nodeid*per*4, buf, per*4, MPI_UNSIGNED_CHAR, MPI_STATUS_IGNORE);
}

void writeOutput_basic(std::fstream& fs, unsigned int outdegree, unsigned int *result, int num_rec, int num_found, int nodeid) {
    int num = 1;
    bool islittleendian = false;
    if (*(char *)&num == 1) {
        islittleendian = true;
    }
    
    int per = 2*num_rec+1;
    fs.seekp(nodeid*per*4);
    unsigned char buffer[4];
    memcpy(buffer, (char*)&outdegree, 4);
    convertEndian(buffer, islittleendian);
    fs.write((char *)buffer, 4);
    for(int i=0;i<num_rec;i++) {
        unsigned int node = i<num_found?result[i*2]:1314212940;
        memcpy(buffer, (char*)&node, 4);
        convertEndian(buffer, islittleendian);
        fs.write((char *)buffer, 4);
        unsigned int infs = i<num_found?result[i*2+1]:1314212940;
        memcpy(buffer, (char*)&infs, 4);
        convertEndian(buffer, islittleendian);
        fs.write((char *)buffer, 4);
    }

    // unsigned int null_number = 1314212940;
    // memcpy(buffer, (char*)&null_number, 4);
    // convertEndian(buffer, islittleendian);
    // for(int i=num_found;i<num_rec;i++) {
    //     fs.write((char *)buffer, 4);
    //     fs.write((char *)buffer, 4);
    // }
}


void convertOutput(int num_nodes, int num_rec) {
    int num = 1;
    bool islittleendian = false;
    if (*(char *)&num == 1) {
        islittleendian = true;
    }

    std::fstream fsr("output.dat", std::ios::in | std::ios::binary);
    std::ofstream fsw("converted.txt", std::ios::out);

    for(int i=0;i<num_nodes;i++) {
        unsigned char buffer[4];
        fsr.read((char *)buffer, 4);
        int num = convertEndian(buffer, islittleendian);
        fsw<<i<<" : "<<num<<"\n";
        for(int j=0;j<num_rec;j++) {
            fsr.read((char *)buffer, 4);
            if(buffer[0]=='N') {
                fsw<<buffer[0]<<buffer[1]<<buffer[2]<<buffer[3]<<" : ";
                fsr.read((char *)buffer, 4);
                fsw<<buffer[0]<<buffer[1]<<buffer[2]<<buffer[3]<<"\n";
            } else {
                num = convertEndian(buffer, islittleendian);
                fsw<<num<<" : ";
                fsr.read((char *)buffer, 4);
                num = convertEndian(buffer, islittleendian);
                fsw<<num<<"\n";
            }
        }
    }
}