#include <stdio.h>
#include <unistd.h>
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
        printf("Rank0 has just sent\n");
    }
    else if (rank == 1)
    {
        sleep(5);
        MPI_Recv(&x, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        printf("Rank1's received from %d with TAG%d...\n", status.MPI_SOURCE, status.MPI_TAG);
        printf("x=%d", x);
    }

    MPI_Finalize();

    return 0;
}