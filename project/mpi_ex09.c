/*
  #define COL 6;
  #define ROW 9;
  を使って6x9の2次元配列を準備する
  Rank0はこの2次元配列に
    10 11 12 13... 18
    20 21 22 23...
    30
    40
  のように代入しておく。
  次にコマンドラインで指定した左上隅の位置と縦横のサイズ似合わせて、Rank0からRank1にそのブロックを送信し、
  Rank1は受け取ったブロックはその値で、それ以外の場所は0で表示しなさい。
  例として、
  $ mpiexec -n 2 ./mpi-ex03 2 3 2 4
    0  0  0  0  0  0  0  0  0
    0  0  0  0  0  0  0  0  0
    0  0  0 33 34 35 36  0  0
    0  0  0 43 44 45 46  0  0
    0  0  0  0  0  0  0  0  0
    0  0  0  0  0  0  0  0  0
  となる。
*/
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define COL 6
#define ROW 9

int main(int argc, char *argv[])
{
  int y = atoi(argv[1]);
  int x = atoi(argv[2]);
  int array_y = atoi(argv[3]);
  int array_x = atoi(argv[4]);
  int array[COL][ROW] = {0};
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Status status;
  MPI_Datatype newtype;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Type_vector(array_y, array_x, ROW, MPI_INT, &newtype);
  MPI_Type_commit(&newtype);

  if (rank == 0)
  {
    for (int i = 0; i < COL; i++)
      for (int j = 0; j < ROW; j++)
        array[i][j] = (i + 1) * 10 + j;

    MPI_Send(&array[y][x], 1, newtype, 1, 10, MPI_COMM_WORLD);
  }
  else if (rank == 1)
  {
    MPI_Recv(&array[y][x], 1, newtype, 0, 10, MPI_COMM_WORLD, &status);

    for (int i = 0; i < COL; i++)
    {
      for (int j = 0; j < ROW; j++)
        printf("%3d ", array[i][j]);
      printf("\n");
    }
  }

  MPI_Type_free(&newtype);
  MPI_Finalize();
  return 0;
}