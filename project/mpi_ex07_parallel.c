// 演習1
// (1) 50000個の整数乱数(< 50000)を配列に生成し、その中身を照準にソート(Bubble Sort)して結果を出力(1部でよい、先頭100個と最後の100個など)する逐次プログラムを作成しなさい。
// (2) 50000個の整数乱数(< 50000)をRank0で配列に生成し、その後、Rank1と協力して、配列の中身を照準にソート(Bubble Sort)する並列プログラムを書きなさい。
// 最大値(LMT=50000)とデータ数(NUM=50000)はコマンドライン引数に渡す

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/time.h>

void print_array(int NUM, int a[])
{
  printf("先頭100個:\n");
  for (int i = 0; i < 100; i++)
  {
    printf("%d ", a[i]);
  }
  printf("\n");

  printf("最後100個:\n");
  for (int i = NUM - 100; i < NUM; i++)
  {
    printf("%d ", a[i]);
  }
  printf("\n");
}

double get_time()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return ((double)(tv.tv_sec) + (double)(tv.tv_usec) * 1e-6);
}

int main(int argc, char *argv[])
{
  int LMT = atoi(argv[1]);
  int NUM = atoi(argv[2]);
  double start_time, end_time;
  int rank, size;
  int parallel_array[NUM];
  MPI_Init(&argc, &argv);
  MPI_Status status;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  if (rank == 0)
  {
    for (int i = 0; i < NUM; i++)
      parallel_array[i] = rand() % LMT;

    printf("===== before sort =====\n");
    print_array(NUM, parallel_array);

    start_time = get_time();

    int j_position = NUM - 1;
    for (int i = 0; i < NUM; i++)
    {
      if (parallel_array[i] <= NUM / 2)
        continue;
      if (i > j_position)
        break;
      for (int j = j_position; j > i; j--)
      {
        if (parallel_array[j] < NUM / 2)
        {
          int tmp = parallel_array[i];
          parallel_array[i] = parallel_array[j];
          parallel_array[j] = tmp;

          j_position = j - 1;

          break;
        }
      }
    }
    printf("===== after swap =====\n");
    print_array(NUM, parallel_array);

    MPI_Send(parallel_array, j_position + 1, MPI_INT, 1, 11, MPI_COMM_WORLD);

    // sort
    for (int i = j_position; i < NUM - 1; i++)
    {
      for (int j = j_position; j < NUM - 1; j++)
      {
        if (parallel_array[j] > parallel_array[j + 1])
        {
          int tmp = parallel_array[j];
          parallel_array[j] = parallel_array[j + 1];
          parallel_array[j + 1] = tmp;
        }
      }
    }

    MPI_Recv(parallel_array, j_position, MPI_INT, 1, 12, MPI_COMM_WORLD, &status);

    end_time = get_time();

    printf("===== after sorted =====\n");
    print_array(NUM, parallel_array);

    printf("並列処理時間:%lf[s]\n", end_time - start_time);
  }
  else if (rank == 1)
  {
    int count;

    MPI_Recv(parallel_array, NUM, MPI_INT, 0, 11, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_INT, &count);

    // sort
    for (int i = 0; i < count - 1; i++)
    {
      for (int j = 0; j < count - 1; j++)
      {

        if (parallel_array[j] > parallel_array[j + 1])
        {
          int tmp = parallel_array[j];
          parallel_array[j] = parallel_array[j + 1];
          parallel_array[j + 1] = tmp;
        }
      }
    }

    MPI_Send(parallel_array, count - 1, MPI_INT, 0, 12, MPI_COMM_WORLD);
  }

  MPI_Finalize();

  return 0;
}

/*
  逐次実行と並列処理の処理時間の考察
  要素数800~1000までは逐次の方が高速。それ以降は2次関数的に並列処理がはやい。
  CPUで2倍より高速化される現象を加速異常(acceleration anomaly)という。
*/
