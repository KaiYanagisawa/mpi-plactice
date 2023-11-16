#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#define N 50

int main(int argc, char *argv[])
{
  int size, rank;
  int x[N], even[N];
  int i;
  MPI_Status status;
  MPI_Request req;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0)
  {
    for (i = 0; i < N; i++)
      x[i] = i * 2;
    printf("Data Ready!!\n");
    fflush(stdout);
    sleep(10);
    printf("Sending ... Start!\n");
    fflush(stdout);
    MPI_Ssend(x, N, MPI_INT, 1, 1, MPI_COMM_WORLD);
    printf("Sending ... Over!!\n");
    fflush(stdout);
  }
  else
  {
    int flag, st = 0;
    while (MPI_Iprobe(0, 1, MPI_COMM_WORLD, &flag, &status), !flag)
    {
      sleep(1);
      printf("#1 sleeping in %dsec...\n", ++st);
      fflush(stdout);
    }
    MPI_Recv(even, N, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);
    printf("#1 received!!\n");
    for (i = 0; i < 50; i++)
    {
      printf("%5d", even[i]);
      if ((i + 1) % 10 == 0)
        printf("\n");
    }
  }

  MPI_Finalize();
  return 0;
}
