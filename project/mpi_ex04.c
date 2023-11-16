#include <stdio.h>
#include <unistd.h>
#include <mpi.h>

#define N 100

int main(int argc, char *argv[])
{
    int rank, size;
    int x;
    int int_array[N];
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        for (int i = 0; i < 40; i++)
            int_array[i] = i;
        
        MPI_Send(int_array, 40, MPI_INT, 1, 11, MPI_COMM_WORLD);
    }
    else if (rank == 1)
    {
        int count;
        MPI_Recv(int_array, N, MPI_INT, 0, 11, MPI_COMM_WORLD, &status);
        MPI_Get_count(&status, MPI_INT, &count);
        for (int i = 0; i < count; i++)
            printf("%d ", int_array[i]);
        printf("\n");
    }

    MPI_Finalize();

    return 0;
}