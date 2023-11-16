#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[])
{
    int rank, size;
    int x;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
    {
        x = 123;
        MPI_Send(&x, 1, MPI_INT, 1, 11, MPI_COMM_WORLD);
    }
    else if (rank == 1)
    {
        MPI_Recv(&x, 1, MPI_INT, 0, 11, MPI_COMM_WORLD, &status);
        printf("x=%d", x);
    }

    MPI_Finalize();

    return 0;
}