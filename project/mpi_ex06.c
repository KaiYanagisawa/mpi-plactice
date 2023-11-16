#include <stdio.h>
#include <unistd.h>
#include <mpi.h>

void zzz(int sec, int rank)
{
  for (int i = 0; i < sec; i++)
  {
    sleep(1);
    printf("#%d is sleeping(%dsec) ...\n", rank, i + 1);
  }
}

int main(int argc, char *argv[])
{
  int rank, size;
  int x;
  MPI_Status status;
  MPI_Request request;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0)
  {
    x = 123;
    zzz(5, rank);
    printf("Rank0 is sending a message ...\n");

    MPI_Send(&x, 1, MPI_INT, 1, 11, MPI_COMM_WORLD);
    // MPI_Wait(&request, &status);

    printf("Rank0 has just sent\n");
  }
  else if (rank == 1)
  {
    int flag;

    printf("Rank1 is waiting for a message ...\n");
    fflush(stdout);

    MPI_Irecv(&x, 1, MPI_INT, 0, 11, MPI_COMM_WORLD, &request);

    // zzz(10, rank);
    do
    {
      zzz(1, rank);
      MPI_Test(&request, &flag, &status);
    } while (!flag);

    // if(!flag) MPI_Wait(&request, &status);
    // MPI_Recv(&x, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    // printf("Rank1's received from %d with TAG%d...\n", status.MPI_SOURCE, status.MPI_TAG);
    printf("Rank1 has x=%d", x);
  }

  MPI_Finalize();

  return 0;
}