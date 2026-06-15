#include "mpi.h"
#include <iostream>

using namespace std;

const int n = 100; // try to increase n -->> Per piccoli n (fino a 1000) funziona anche con Send e Receive normali, ma per grandi n è necessario utilizzare Isend e Irecv, altrimenti si rischia di incorrere in un deadlock

int main(int argc, char* argv[]){
    int size, rank;

    MPI_Init(&argc,&argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Status stat1, stat2;
    MPI_Request req;

    int* imesg = new int[n]; 
    int* imesg2 = new int[n];
    
    int itag=1; int itag2=2;

    for(int i=0;i<n;i++){
        imesg[i]=rank; 
        imesg2[i]=rank+1;
    }

    if(rank==1){
        MPI_Isend(&imesg[0], n, MPI_INTEGER, 0, itag, MPI_COMM_WORLD, &req);
        MPI_Recv(&imesg2[0], n, MPI_INTEGER, 0, itag2, MPI_COMM_WORLD, &stat2);
        MPI_Wait(&req, &stat1);
        cout << "messaggio = " << imesg2[0] << endl;
    }else if(rank==0){
        MPI_Send(&imesg2[0], n, MPI_INTEGER, 1, itag2, MPI_COMM_WORLD);
        MPI_Recv(&imesg[0], n, MPI_INTEGER, 1, itag, MPI_COMM_WORLD, &stat1);
        cout<<"messaggio = "<<imesg[0]<<endl;
    }

    MPI_Finalize();

return 0;}