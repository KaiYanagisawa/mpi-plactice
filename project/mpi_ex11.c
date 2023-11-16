/*
  最大100人の学生データ(学籍番号、妙名、３科目の得点)から、最高得点を全員分表示する並列プログラムを作成
  Rank0がstudentData.txtにあるデータを全て読み込み、それを他のRankに分配する(できるだけ均等化する)。
  最終的に全ての最高得点者が表示されるのであれば、Rank0がまとめて表示しても、全Rankで表示してもどちらでも良いものとする

  ===== 参考 =====
  char fname[40];
  scanf("%s", fname);
  fd = fopen(fname, "r");
  ================

  ※ ファイルオープン時のエラー処理でexit 1をしてはいけない。
    MPI_Finalizeで行う全てのプロセスが最後に解散する必要があるため
    MPI_Abort(MPI_COMM_WORLD, 1);でMPIのrankをexit 1をするように記述する必要がある。
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define STUDENT_NUMBER 100
#define ID_LENGTH      8
#define NAME_LENGTH    10

struct Student
{
  char id[ID_LENGTH];
  char name[NAME_LENGTH];
  int  point[4]; // 合計得点のための4つ目を含む
} students[STUDENT_NUMBER];

int main(int argc, char *argv[])
{
  int  rank, size;

  char id[ID_LENGTH], name[NAME_LENGTH];
  int  point1, point2, point3;

  struct Student *receive_max_students;
  receive_max_students = (struct Student *)malloc(sizeof(struct Student) * STUDENT_NUMBER);

  MPI_Status status;
  MPI_Datatype mytype;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  int block_lengths[4] = {8, 10, 4, 1};
  MPI_Aint displacements[4];
  MPI_Aint base;
  MPI_Address(students[0].id, &displacements[0]);
  MPI_Address(students[0].name, &displacements[1]);
  MPI_Address(students[0].point, &displacements[2]);
  MPI_Address(students[1].id, &displacements[3]);
  base = displacements[0];
  for (int i = 0; i < 4; i++)
    displacements[i] -= base;
  MPI_Datatype dtypes[4] = {MPI_CHAR, MPI_CHAR, MPI_INT, MPI_UB};
  MPI_Type_struct(4, block_lengths, displacements, dtypes, &mytype);
  MPI_Type_commit(&mytype);

  if (rank == 0)
  {
    struct Student *max_point_students;
    max_point_students = (struct Student *)malloc(sizeof(struct Student) * STUDENT_NUMBER);
    int max_student_length;

    int max;

    int number_per_rank;
    int surplus; 
    int rank0_student_number;

    int input_student_number = 0;

    int found_max_rank_number = 0;
    int *found_max_ranks = (int *)malloc(sizeof(int) * size);

    FILE *student_grades;
    student_grades = fopen("studentData.txt", "r");
    for (int i = 0;\
         i < STUDENT_NUMBER && \
         fscanf(student_grades, "%s%s%d%d%d", id, name, &point1, &point2, &point3) != EOF;\
         i++)
    {
      int grade_sum = point1 + point2 + point3;

      strcpy(students[i].id, id);
      strcpy(students[i].name, name);
      students[i].point[0] = point1;
      students[i].point[1] = point2;
      students[i].point[2] = point3;
      students[i].point[3] = grade_sum;

      input_student_number++;
    }
    fclose(student_grades);

    number_per_rank      = input_student_number / size;
    surplus              = input_student_number % size;
    rank0_student_number = number_per_rank + ((surplus > 0) ? 1 : 0);

    for (int i = 1; i < size; i++)
      MPI_Send(students + number_per_rank * i + ((i < surplus) ? i : surplus),
               number_per_rank + ((i < surplus) ? 1 : 0),
               mytype,
               i,
               10,
               MPI_COMM_WORLD);

    max = students[0].point[3];
    for (int i = 1; i < rank0_student_number; i++)
      if (students[i].point[3] > max)
        max = students[i].point[3];

    max_student_length = 0;
    for (int i = 0; i < rank0_student_number; i++)
      if (students[i].point[3] == max)
        max_point_students[max_student_length++] = students[i];

    printf("Rank 0 found\n");
    for (int i = 0; i < max_student_length; i++)
    {
      printf("%8s%10s%4d%4d%4d%4d\n",
             max_point_students[i].id,
             max_point_students[i].name,
             max_point_students[i].point[0],
             max_point_students[i].point[1],
             max_point_students[i].point[2],
             max_point_students[i].point[3]);
    }
    found_max_rank_number = 1;
    found_max_ranks[0] = rank; 

    for (int i = 1; i < size; i++)
    {
      int count;

      MPI_Recv(receive_max_students,
               STUDENT_NUMBER,
               mytype,
               MPI_ANY_SOURCE,
               20,
               MPI_COMM_WORLD,
               &status);
      MPI_Get_count(&status, mytype, &count);

      printf("Rank%2d found\n", status.MPI_SOURCE);
      for (int i = 0; i < count; i++)
      {
        printf("%8s%10s%4d%4d%4d%4d\n",
               receive_max_students[i].id,
               receive_max_students[i].name,
               receive_max_students[i].point[0],
               receive_max_students[i].point[1],
               receive_max_students[i].point[2],
               receive_max_students[i].point[3]);
      }

      if (receive_max_students[0].point[3] > max_point_students[0].point[3])
      {
        for (int j = 0; j < max_student_length; j++)
        {
          struct Student empty_student;
          max_point_students[j] = empty_student;
        }
        max_student_length = 0;

        for (int j = 0; j < count; j++)
          max_point_students[max_student_length++] = receive_max_students[j];

        found_max_rank_number = 1;
        found_max_ranks[0] = status.MPI_SOURCE;
      }
      else if (receive_max_students[0].point[3] == max_point_students[0].point[3])
      {
        for (int j = 0; j < count; j++)
          max_point_students[max_student_length++] = receive_max_students[j];

        found_max_rank_number++;
        found_max_ranks[found_max_rank_number - 1] = status.MPI_SOURCE;
      }
    }

    printf("Maximum points\n");
    for (int i = 0; i < max_student_length; i++)
    {
      printf("%8s%10s%4d%4d%4d%4d\n",
             max_point_students[i].id,
             max_point_students[i].name,
             max_point_students[i].point[0],
             max_point_students[i].point[1],
             max_point_students[i].point[2],
             max_point_students[i].point[3]);
    }

    printf("It was found by rank ");
    for (int i = 0; i < found_max_rank_number; i++)
      printf("%d ", found_max_ranks[i]);
    printf("\n");

    free(max_point_students);
    free(found_max_ranks);
  }
  else
  {
    int count;
    int max;
    int receive_length;

    MPI_Recv(students, STUDENT_NUMBER, mytype, 0, 10, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, mytype, &count);

    printf("Rank%d %d received\n", rank, count);
    for (int i = 0; i < count; i++)
      printf("%8s%10s%4d%4d%4d%4d\n",
             students[i].id,
             students[i].name,
             students[i].point[0],
             students[i].point[1],
             students[i].point[2],
             students[i].point[3]);

    max = students[0].point[3];
    for (int i = 1; i < count; i++)
      if (students[i].point[3] > max)
        max = students[i].point[3];

    receive_length = 0;
    for (int i = 0; i < count; i++)
      if (students[i].point[3] == max)
        receive_max_students[receive_length++] = students[i];

    MPI_Send(receive_max_students, receive_length, mytype, 0, 20, MPI_COMM_WORLD);
  }

  MPI_Type_free(&mytype);
  MPI_Finalize();

  free(receive_max_students);

  return 0;
}
