#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
  MPI_Init(&argc, &argv);

  printf("Hello, %s!!\n", argv[1]);

  MPI_Finalize();
  return 0;
}
