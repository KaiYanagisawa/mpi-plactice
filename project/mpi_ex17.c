/*
  ex16のプログラムのMPI_Scatter後の分割されたデータをrank0の
  配列aにMPI_Gatherする動作を確認するプログラムを作成しなさい。
  MPI_Gather後の配列aの内容をrank0が表示すること
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

  for (int i = 0; i < 24; i++)
  {
    a[i] = 0;
  }
  MPI_Gather(&b, 24 / size, MPI_INT, &a, 24 / size, MPI_INT, 0, MPI_COMM_WORLD);

  if (rank == 0)
  {
    printf("Gather rank%d\n", rank);
    for (int i = 0; i < 24; i++)
    {
      printf("%3d", a[i]);
    }
    printf("\n");
  }

  MPI_Finalize();
}