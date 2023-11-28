/*
  ランク0が24個の整数配列a[]をもち、その要素にa[i]=i+1を設定する。
  その後,MPI_Scatterを用いて他のランク(24の約数に当たる個数のランク数を使うものとする)に、
  a[]の要素を分割して渡し(受け取る側はb[]という配列に受け取る)
  それぞれのランクで受け取ったデータを表示するプログラムを作成しなさい。
  念のため、配列a[], b[]の使わない部分は0で初期化しておくこと。int a[24]=[0], b[24]=[0];
*/
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
  int rank, size;
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int a[24] = {0};
  int b[24] = {0};
  if (rank == 0)
  {
    for (int i = 0; i < 24; i++)
    {
      a[i] = i + 1;
    }
  }
  MPI_Scatter(&a, 24 / size, MPI_INT, &b, 24 / size, MPI_INT, 0, MPI_COMM_WORLD);
  printf("rank%d\n", rank);
  for (int i = 0; i < 24; i++)
  {
    printf("%3d", b[i]);
  }
  printf("\n");

  MPI_Finalize();
}
