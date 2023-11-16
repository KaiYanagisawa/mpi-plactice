#include <stdio.h>
#include <unistd.h>
#include <mpi.h>

void zzz(int sec, int rank)
{
  for(int i=0; i<sec; i++){
    sleep(1);
    printf("#%d is sleeping(%dsec) ...\n", rank,i+1);
  }
}

int main(int argc, char *argv[])
{
  int x;
  int rank, size;
  MPI_Status status;
  MPI_Request request;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  if(rank==0){
    x=12345;
    zzz(8,rank);
    printf("Rank0 is sending a message ...\n");
    MPI_Send(&x,1,MPI_INT,1,11,MPI_COMM_WORLD);
    printf("Rank0 has just sent!!\n");
  } else {
    int flag=0, i;
    printf("Rank1 is waiting for a message ...\n"); fflush(stdout);
    MPI_Irecv(&x,1,MPI_INT,0,11,MPI_COMM_WORLD,&request);
    for(i=0; !flag&&i<3; i++){
      zzz(1,rank);
      MPI_Test(&request, &flag, &status);
    }
    if(!flag) MPI_Wait(&request,&status);
    printf("Rank1 has x=%d\n", x);
  }
  MPI_Finalize();
  return 0;
}
