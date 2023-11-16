#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#define N 100

int main(int argc, char *argv[])
{
  int x[100];
  int rank, size;
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  if(rank==0){
    for(int i=0; i<40; i++) x[i]=i;
    MPI_Send(x,40,MPI_INT,1,11,MPI_COMM_WORLD);
  } else {
    int count;
    MPI_Recv(x,N,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
    MPI_Get_count(&status,MPI_INT,&count);
    printf("Rank1's received from %d with TAG%d...\n",status.MPI_SOURCE, status.MPI_TAG);
    for(int i=0; i<count; i++)
      printf("%4d ",x[i]);
    printf("\n");
  }
  MPI_Finalize();
  return 0;
}
