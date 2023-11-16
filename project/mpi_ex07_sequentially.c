// 演習1
// (1) 50000個の整数乱数(< 50000)を配列に生成し、その中身を照準にソート(Bubble Sort)して結果を出力(1部でよい、先頭100個と最後の100個など)する逐次プログラムを作成しなさい。
// (2) 50000個の整数乱数(< 50000)をRank0で配列に生成し、その後、Rank1と協力して、配列の中身を照準にソート(Bubble Sort)する並列プログラムを書きなさい。
// 最大値(LMT=50000)とデータ数(NUM=50000)はコマンドライン引数に渡す

#include <stdio.h>
#include <stdlib.h>
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
  int a[NUM];
  double start_time, end_time;

  for (int i = 0; i < NUM; i++)
  {
    a[i] = (int)rand() % LMT;
  }

  start_time = get_time();
  // バブルソート
  for (int i = 0; i < NUM - 1; i++)
  {
    for (int j = 0; j < NUM - 1; j++)
    {
      if (a[j] > a[j + 1])
      {
        int tmp = a[j];
        a[j] = a[j + 1];
        a[j + 1] = tmp;
      }
    }
  }
  end_time = get_time();

  printf("(1)\n");
  print_array(NUM, a);
  printf("逐次処理時間:%lf[s]\n", end_time - start_time);

  return 0;
}
