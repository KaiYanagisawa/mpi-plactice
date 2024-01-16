#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#define N 100

struct student
{
  char stID[8];
  char name[10];
  int point[4];
};
typedef struct student Student;

int main(int argc, char *argv[])
{
  int size, rank;
  MPI_Status status;
  Student st[N], top[20];
  int nc = 0;            /* nc:学生数  */
  int max = 0, nmax = 0; /* 最高得点とその人数 */
  int i, j;
  unsigned int membersize, maxsize;
  int position;
  void *buf;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Pack_size(1, MPI_INT, MPI_COMM_WORLD, &membersize);
  maxsize = membersize;
  for (i = 0; i < N; i++)
  {
    MPI_Pack_size(8, MPI_CHAR, MPI_COMM_WORLD, &membersize); // stIDの分
    maxsize += membersize;
    MPI_Pack_size(10, MPI_CHAR, MPI_COMM_WORLD, &membersize); // nameの分
    maxsize += membersize;
    MPI_Pack_size(4, MPI_INT, MPI_COMM_WORLD, &membersize); // pointの分
    maxsize += membersize;
  }
  buf = (void *)malloc(maxsize);

  if (rank == 0)
  {
    int NE, NR;
    int starti, ndata;
    int *maxp = (int *)malloc(size * sizeof(int));
    int nc0;  // rank0の担当学生数
    int maxv; // 他のrankからの最高得点
    FILE *fp;
    if (!(fp = fopen(argv[1], "r")))
    {
      fprintf(stderr, "!?!? Can't open the file:%s\n", argv[1]);
      MPI_Abort(MPI_COMM_WORLD, 1);
    }
    while (fscanf(fp, "%s%s", st[nc].stID, st[nc].name) != EOF)
    {
      for (i = 0; i < 3; i++)
        fscanf(fp, "%d", &st[nc].point[i]);
      nc++;
    } // rank0がstudentData.txtからすべてのdataを読む
    fclose(fp);

    NE = nc / size;
    NR = nc % size;
    // printf("全学生数：%d名, NE=%d, NR=%d\n", nc,NE,NR); fflush(stdout);
    for (i = 1; i < size; i++)
    {
      starti = NE * i + ((NR > i) ? i : NR);
      ndata = NE + ((NR > i) ? 1 : 0);
      position = 0;
      MPI_Pack(&ndata, 1, MPI_INT, buf, maxsize, &position, MPI_COMM_WORLD);
      for (int m = starti; m < starti + ndata; m++)
      {
        MPI_Pack(st[m].stID, 8, MPI_CHAR, buf, maxsize, &position, MPI_COMM_WORLD);
        MPI_Pack(st[m].name, 10, MPI_CHAR, buf, maxsize, &position, MPI_COMM_WORLD);
        MPI_Pack(st[m].point, 4, MPI_INT, buf, maxsize, &position, MPI_COMM_WORLD);
      }
      MPI_Send(buf, position, MPI_PACKED, i, 11, MPI_COMM_WORLD);
    } // 他のrankにdataを送信
    nc0 = nc / size + ((nc % size > 0) ? 1 : 0);
    for (i = 0; i < nc0; i++)
    {
      st[i].point[3] = st[i].point[0] + st[i].point[1] + st[i].point[2];
      if (max < st[i].point[3])
        max = st[i].point[3];
    }
    // すべてのrankから最大値を集め、全体の最大値を求める
    maxp[0] = max;
    for (i = 1; i < size; i++)
    {
      MPI_Recv(&maxv, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
      maxp[status.MPI_SOURCE] = maxv;
      if (max < maxv)
        max = maxv;
    }
    printf("Each max-->");
    for (i = 0; i < size; i++)
      printf("%4d", maxp[i]);
    printf("\n");
    // 真の最大値を他のrankに知らせる
    for (i = 1; i < size; i++)
      MPI_Send(&max, 1, MPI_INT, i, 12, MPI_COMM_WORLD);
    // 自分の担当学生の中の最大得点者を探す
    for (i = 0; i < nc0; i++)
      if (st[i].point[3] == max)
        top[nmax++] = st[i];
    printf("Rank:%d Top List ---(%d)-----\n", rank, nmax);
    for (i = 0; i < nmax; i++)
      printf("%s %-10s %3d %3d %3d %3d\n", top[i].stID, top[i].name,
             top[i].point[0], top[i].point[1], top[i].point[2], top[i].point[3]);
    free(maxp);
  }
  else
  {
    int msgsize;
    MPI_Recv(buf, maxsize, MPI_PACKED, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    position = 0;
    MPI_Get_count(&status, MPI_PACKED, &msgsize);
    MPI_Unpack(buf, msgsize, &position, &nc, 1, MPI_INT, MPI_COMM_WORLD);
    for (int m = 0; m < nc; m++)
    {
      MPI_Unpack(buf, msgsize, &position, st[m].stID, 8, MPI_CHAR, MPI_COMM_WORLD);
      MPI_Unpack(buf, msgsize, &position, st[m].name, 10, MPI_CHAR, MPI_COMM_WORLD);
      MPI_Unpack(buf, msgsize, &position, st[m].point, 4, MPI_INT, MPI_COMM_WORLD);
    }
    printf("Rank%d received %d students data...\n", rank, nc);
    fflush(stdout);
    for (i = 0; i < nc; i++)
    {
      st[i].point[3] = st[i].point[0] + st[i].point[1] + st[i].point[2];
      if (max < st[i].point[3])
        max = st[i].point[3];
    }
    MPI_Send(&max, 1, MPI_INT, 0, 22, MPI_COMM_WORLD);
    MPI_Recv(&max, 1, MPI_INT, 0, 12, MPI_COMM_WORLD, &status);
    for (i = 0; i < nc; i++)
      if (st[i].point[3] == max)
        top[nmax++] = st[i];
    printf("Rank:%d Top List ---(%d)-----\n", rank, nmax);
    for (i = 0; i < nmax; i++)
      printf("%s %-10s %3d %3d %3d %3d\n", top[i].stID, top[i].name,
             top[i].point[0], top[i].point[1], top[i].point[2], top[i].point[3]);
  }

  MPI_Finalize();
  return 0;
}
/*
[kai@apollo ~/MPI]$ mpiexec -n 4 ./typeStruct studentData.txt
Each max--> 271 271 271 271
Rank:0 Top List --------
S082006 Ohyama      88  92  91 271
S082016 Seguchi     88  92  91 271
Rank:1 Top List --------
S082026 Tobita      88  92  91 271
Rank:2 Top List --------
S082036 Hamada      88  92  91 271
S082046 Mukai       88  92  91 271
Rank:3 Top List --------
S082056 Yokoi       88  92  91 271
S082061 Wakimoto    88  92  91 271
[kai@apollo ~/MPI]$ mpiexec -n 8 ./typeStruct studentData.txt
Each max--> 271 271 256 271 271 271 271 271
Rank:0 Top List --------
S082006 Ohyama      88  92  91 271
Rank:1 Top List --------
S082016 Seguchi     88  92  91 271
Rank:2 Top List --------
Rank:3 Top List --------
S082026 Tobita      88  92  91 271
Rank:4 Top List --------
S082036 Hamada      88  92  91 271
Rank:5 Top List --------
S082046 Mukai       88  92  91 271
Rank:6 Top List --------
S082056 Yokoi       88  92  91 271
Rank:7 Top List --------
S082061 Wakimoto    88  92  91 271
[kai@apollo ~/MPI]$

*/
