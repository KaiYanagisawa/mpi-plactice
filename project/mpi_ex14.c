#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
  int rank, size;
  MPI_Status status;
  int a;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0)
    a = 1234;

  MPI_Bcast(&a, 1, MPI_INT, 0, MPI_COMM_WORLD);

  printf("Rank%d has a=%d\n", rank, a);

  MPI_Finalize();
  return 0;
}