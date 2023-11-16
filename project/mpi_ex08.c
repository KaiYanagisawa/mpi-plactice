#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define LIMIT 10000

int main(int argc, char *argv[])
{
  int N = atoi(argv[1]);

  int rank, size;
  int *number_array;
  int max;

  number_array = (int *)malloc(sizeof(int) * N);

  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0)
  {
    int *found_max_ranks;
    int found_max_rank_number = 0;
    int number_per_rank = N / size;
    int surplus = N % size;
    int count = number_per_rank + ((surplus > 0) ? 1 : 0);
    int big;

    found_max_ranks = (int *)malloc(sizeof(int) * size);

    for (int i = 0; i < N; i++)
    {
      number_array[i] = rand() % LIMIT;
      printf("%4d%c", number_array[i], ((i + 1) % 10 == 0) ? '\n' : ' ');
    }
    printf("\n");

    for (int i = 1; i < size; i++)
      MPI_Send(number_array + number_per_rank * i + ((i < surplus) ? i : surplus),
               number_per_rank + ((i < surplus) ? 1 : 0),
               MPI_INT,
               i,
               10,
               MPI_COMM_WORLD);

    max = number_array[0];
    for (int i = 1; i < count; i++)
      if (max < number_array[i])
        max = number_array[i];

    printf("Rank 0 found %4d.\n", max);

    found_max_rank_number = 1;
    found_max_ranks[0] = rank;

    for (int i = 1; i < size; i++)
    {
      MPI_Recv(&big, 1, MPI_INT, MPI_ANY_SOURCE, 20, MPI_COMM_WORLD, &status);

      printf("Rank%2d found %4d.\n", status.MPI_SOURCE, big);

      if (max < big)
      {
        max = big;
        found_max_rank_number = 1;
        found_max_ranks[0] = status.MPI_SOURCE;
      }
      else if (max == big)
      {
        found_max_rank_number++;
        found_max_ranks[found_max_rank_number - 1] = status.MPI_SOURCE;
      }
    }

    printf("Maximum data is %d!!\n", max);
    printf("  It was found by rank ");
    for (int i = 0; i < found_max_rank_number; i++)
      printf("%2d", found_max_ranks[i]);
    printf("\n");

    free(found_max_ranks);
  }
  else
  {
    int count;

    MPI_Recv(number_array, N, MPI_INT, 0, 10, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_INT, &count);

    max = number_array[0];
    for (int i = 1; i < count; i++)
      if (max < number_array[i])
        max = number_array[i];

    MPI_Send(&max, 1, MPI_INT, 0, 20, MPI_COMM_WORLD);
  }

  MPI_Finalize();

  free(number_array);

  return 0;
}

/*
  $ mpiexec -n 5 ./mpi_ex08 20
  6807 5249   73 3658 8930 1272 7544  878 7923 7709
  4440 8165 4492 3042 7987 2503 2327 1729 8840 2612

  Rank 0 found 6807.
  Rank 4 found 8840.
  Rank 1 found 8930.
  Rank 2 found 8165.
  Rank 3 found 7987.
  Maximum data is 8930!!
    It was found by rank  1
*/
