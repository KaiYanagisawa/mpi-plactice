// 演習2
// int型配列に格納されたデータの合計を2つのrankで協力して求めよ。
// ただし、データの個数は100個以内で10000未満の整数をランダムにrank0で格納しておくこと。
// 答え:503080

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <mpi.h>

#define N 100
#define LMT 10000

int main(int argc, char *argv[])
{
	int rank, size;
	int min = 0, max = 9999;
	int int_array[N];
	int sum = 0;
	int sum2 = 0;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0)
	{
		srand(4);
		// for (int i = 0; i < N; i++)
		// 	int_array[i] = min + (int)(rand() * (max - min + 1.0) / (1.0 + RAND_MAX));
		for (int i = 0; i < N; i++)
			int_array[i] = rand() % LMT;

		for (int i = 0; i < N; i++)
			printf("%d ", int_array[i]);
		printf("\n");

		if (N % 2 == 0)
		{
			MPI_Send(int_array, N / 2, MPI_INT, 1, 11, MPI_COMM_WORLD);
			for (int i = N / 2; i < N; i++)
			{
				sum += int_array[i];
				printf("%d ", int_array[i]);
			}
			printf("\n");
			printf("dev sum= %d\n", sum);
		}
		else
		{
			MPI_Send(int_array, (N + 1) / 2, MPI_INT, 1, 11, MPI_COMM_WORLD);
			for (int i = (N + 1) / 2; i < N; i++)
			{
				sum += int_array[i];
			}
		}

		MPI_Recv(&sum2, 1, MPI_INT, 1, 12, MPI_COMM_WORLD, &status);

		sum += sum2;

		printf("sum=%d\n", sum);
	}
	else if (rank == 1)
	{
		int count;

		MPI_Recv(int_array, N, MPI_INT, 0, 11, MPI_COMM_WORLD, &status);
		MPI_Get_count(&status, MPI_INT, &count);

		for (int i = 0; i < count; i++)
		{
			sum2 += int_array[i];
			printf("%d ", int_array[i]);
		}
		printf("\n");
		MPI_Send(&sum2, 1, MPI_INT, 0, 12, MPI_COMM_WORLD);
	}

	MPI_Finalize();

	return 0;
}