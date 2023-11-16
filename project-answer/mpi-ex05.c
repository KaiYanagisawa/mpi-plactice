#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define N 100
#define LMT 10000

int main(int argc, char *argv[])
{
  int x[100], localsum, count = 0;
  char output[1000];
  int rank, size;
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0)
  {
    int sum;
    printf("Generated integers:\n");
    for (int i = 0; i < 100; i++, count++)
    {
      x[i] = rand() % LMT;
      printf("%5d", x[i]);
    }
    printf("\n");
    MPI_Send(x + count / 2, count / 2 + count % 2, MPI_INT, 1, 11, MPI_COMM_WORLD);
    localsum = 0;
    for (int i = 0; i < count / 2; i++)
    {
      sprintf(output + i * 5, "%5d", x[i]);
      localsum += x[i];
    }
    printf("Rank0--(%d)---\n%s\n", count / 2, output);

    MPI_Recv(&sum, 1, MPI_INT, 1, 22, MPI_COMM_WORLD, &status);
    sum += localsum;
    printf("Total=%d\n", sum);
  }
  else
  {
    MPI_Recv(x, N, MPI_INT, 0, 11, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_INT, &count);
    localsum = 0;
    for (int i = 0; i < count; i++)
    {
      sprintf(output + i * 5, "%5d", x[i]);
      localsum += x[i];
    }
    printf("Rank1--(%d)---\n%s\n", count, output);
    MPI_Send(&localsum, 1, MPI_INT, 0, 22, MPI_COMM_WORLD);
  }
  MPI_Finalize();
  return 0;
}
