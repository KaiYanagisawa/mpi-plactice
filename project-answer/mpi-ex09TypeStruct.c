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
  int blocklength[4] = {8, 10, 4, 1};
  MPI_Aint base, displacement[4];
  MPI_Datatype types[4] = {MPI_CHAR, MPI_CHAR, MPI_INT, MPI_UB};
  MPI_Datatype mytype;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MPI_Address(st[0].stID, &displacement[0]);
  MPI_Address(st[0].name, &displacement[1]);
  MPI_Address(st[0].point, &displacement[2]);
  MPI_Address(st[1].stID, &displacement[3]);
  for (i = 0, base = displacement[0]; i < 4; i++)
    displacement[i] -= base;
  MPI_Type_struct(4, blocklength, displacement, types, &mytype);
  MPI_Type_commit(&mytype);

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
    for (i = 1; i < size; i++)
    {
      starti = NE * i + ((NR > i) ? i : NR);
      ndata = NE + ((NR > i) ? 1 : 0);
      MPI_Send(st + starti, ndata, mytype, i, 11, MPI_COMM_WORLD);
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
    printf("Rank:%d Top List --------\n", rank);
    for (i = 0; i < nmax; i++)
      printf("%s %-10s %3d %3d %3d %3d\n", top[i].stID, top[i].name,
             top[i].point[0], top[i].point[1], top[i].point[2], top[i].point[3]);
    free(maxp);
  }
  else
  {
    MPI_Recv(st, N, mytype, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, mytype, &nc);
    // printf("Rank%d received %d students data...\n",rank,nc); fflush(stdout);
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
    printf("Rank:%d Top List --------\n", rank);
    for (i = 0; i < nmax; i++)
      printf("%s %-10s %3d %3d %3d %3d\n", top[i].stID, top[i].name,
             top[i].point[0], top[i].point[1], top[i].point[2], top[i].point[3]);
  }

  MPI_Type_free(&mytype); // TypeStructの型を開放
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
