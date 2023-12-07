#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <mpi.h>

// #define NUM 50000
// #define MAX 50000
#define NRow 15

double gettime()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return ((double)(tv.tv_sec) + (double)(tv.tv_usec) * 1e-6);
}

int main(int argc, char *argv[])
{
  int rank, size;
  int *data;
  int MAX, NUM;
  int np; // number of data in each rank;
  MPI_Status status;

  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  MAX = atoi(argv[1]);
  NUM = atoi(argv[2]);
  data = (int *)malloc(sizeof(int) * NUM);

  if (rank == 0)
  {
    double stime, etime;
    // Generate Random Integer
    printf("----- Before sort -----\n");
    for (int i = 0; i < NUM; i++)
    {
      data[i] = rand() % MAX;
      if (i < 100)
        printf("%5d%c", data[i], ((i + 1) % NRow == 0) ? '\n' : ' ');
    }

    stime = gettime();
    // Divide using pivot(MAX/2)
    {
      int p = 0, q = NUM - 1;
      while (p < q)
      {
        while (data[p] < MAX / 2)
          p++;
        while (data[q] >= MAX / 2)
          q--;
        if (p < q)
        {
          int tmp = data[p];
          data[p] = data[q];
          data[q] = tmp;
        }
      }
      np = p;
    }
    MPI_Send(data + np, NUM - np, MPI_INT, 1, 10, MPI_COMM_WORLD);

    // Bubble sort
    for (int i = 0; i < np - 1; i++)
    {
      for (int j = 0; j < np - 1; j++)
      {
        if (data[j] > data[j + 1])
        {
          int tmp = data[j];
          data[j] = data[j + 1];
          data[j + 1] = tmp;
        }
      }
    }
    MPI_Recv(data + np, NUM - np, MPI_INT, 1, 20, MPI_COMM_WORLD, &status);
    etime = gettime();

    printf("\n----- After Sort around index(%d) --%lfsec---\n", np, etime - stime);
    for (int i = np - 20; i < np + 20; i++)
      printf("%5d%c", data[i], (i == np - 1) ? '*' : (((i + 1) % NRow == 0) ? '\n' : ' '));
    printf("\n");
  }
  else
  {
    MPI_Recv(data, NUM, MPI_INT, 0, 10, MPI_COMM_WORLD, &status);
    MPI_Get_count(&status, MPI_INT, &np);
    // Bubble sort
    for (int i = 0; i < np - 1; i++)
    {
      for (int j = 0; j < np - 1; j++)
      {
        if (data[j] > data[j + 1])
        {
          int tmp = data[j];
          data[j] = data[j + 1];
          data[j + 1] = tmp;
        }
      }
    }
    MPI_Send(data, np, MPI_INT, 0, 20, MPI_COMM_WORLD);
  }

  MPI_Finalize();
  return 0;
}
