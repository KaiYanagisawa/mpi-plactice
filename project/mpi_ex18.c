/*
  要素数10の配列aを各ランクに準備しておく。各ランクにおいて、10個の1桁の乱数を配列aに格納し、
  その内容を表示する。その後MPI_Reduceによりランク0にMPI_MAX, MPI_MIN, MPI_SUM, MPI_PROD
  などの演算結果を求めるプログラムを作成しなさい。
  乱数生成の際は、各ランクの乱数を変えるため、srand((rank + 1) * 13);の種を使ってみよう
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define N 10

int main(int argc, char *argv[])
{
  int rank, size;
  MPI_Status status;
  int a[N] = {0};
  int b[4] = {0};

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  srand((rank + 1) * 13);
  printf("rank%d\n", rank);
  for (int i = 0; i < N; i++)
  {
    a[i] = rand() % 10;
    printf("%d ", a[i]);
  }
  printf("\n");

  MPI_Reduce(&a, &b, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
  MPI_Reduce(&a, &b[1], 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);
  MPI_Reduce(&a, &b[2], 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  MPI_Reduce(&a, &b[3], 1, MPI_INT, MPI_PROD, 0, MPI_COMM_WORLD);

  
  if (rank == 0)
  {
    printf("reduce rank0\n");
    for (int i = 0; i < 4; i++)
    {
      printf("%4d", b[i]);
    }
  }

  MPI_Finalize();
}
