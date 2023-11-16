#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define N 1000 // データ数
#define M 700  // 最大値

int main(int argc, char *argv[])
{
  int rank, size;
  int data[N];
  int max;
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0)
  {
    int *rnkList;                       // 同じ最大値をみつけたrankのリスト
    int nL = 0;                         // 同じ最大値をみつけたrankの個数
    int n = N / size;                   // データの等分数
    int nr = N % size;                  // 1個余計に担当するランク数
    int count = n + ((nr > 0) ? 1 : 0); // rank0の担当データ数
    int big;

    rnkList = (int *)malloc(sizeof(int) * size);

    for (int i = 0; i < N; i++)
    { // データ生成
      data[i] = rand() % M;
      printf("%4d%c", data[i], ((i + 1) % 10 == 0) ? '\n' : ' ');
    }
    printf("\n");

    for (int i = 1; i < size; i++) // データ分配
      MPI_Send(data + n * i + ((i < nr) ? i : nr), n + ((i < nr) ? 1 : 0), MPI_INT, i, 10, MPI_COMM_WORLD);

    max = data[0];
    for (int i = 1; i < count; i++)
      if (max < data[i])
        max = data[i];
    printf("Rank 0 found %4d.\n", max);
    nL = 1;
    rnkList[0] = rank;

    for (int i = 1; i < size; i++)
    { // 最大値回収
      MPI_Recv(&big, 1, MPI_INT, MPI_ANY_SOURCE, 20, MPI_COMM_WORLD, &status);
      printf("Rank%2d found %4d.\n", status.MPI_SOURCE, big);
      if (max < big)
      {
        max = big;
        nL = 1;
        rnkList[0] = status.MPI_SOURCE;
      }
      else if (max == big)
      {
        nL++;
        rnkList[nL - 1] = status.MPI_SOURCE;
      }
    }
    printf("Maximum data is %d!!\n", max);
    printf("  It was found by rank ");
    for (int i = 0; i < nL; i++)
      printf("%d,", rnkList[i]);
    printf("\n");
  }
  else
  {
    int count;
    MPI_Recv(data, N, MPI_INT, 0, 10, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_INT, &count);
    max = data[0];
    for (int i = 1; i < count; i++)
      if (max < data[i])
        max = data[i];
    MPI_Send(&max, 1, MPI_INT, 0, 20, MPI_COMM_WORLD);
  }
  MPI_Finalize();

  return 0;
}
