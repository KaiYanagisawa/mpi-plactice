#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD,&size);
  MPI_Comm_rank(MPI_COMM_WORLD,&rank);
  if(rank%2==0) // 偶数rankのみが出⼒
    printf("Hello!!!\n");
  MPI_Finalize();
  return 0;
}
