#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
  int x;
  int rank, size;
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  if(rank==0){
    x=12345;
    MPI_Send(&x,1,MPI_INT,1,11,MPI_COMM_WORLD);
  } else {
    MPI_Recv(&x,1,MPI_INT,0,11,MPI_COMM_WORLD,&status);
    printf("Rank1 has x=%d\n", x);
  }
  MPI_Finalize();
  return 0;
}
