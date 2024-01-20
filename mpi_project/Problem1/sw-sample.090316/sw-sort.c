#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <mpi.h>

typedef struct
{
  char query_sequence[128];
  char database_sequence[128];
  int bestScore;
  char q[512];
  char d[512];
} output_info;

int main(int argc, char **argv)
{
	char *file_output;
  int rank, size;
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  assert(argc >= 2);
  file_output = argv[1];

  if (rank == 0)
  {
    char line[LINE_MAX];
    int count = 0;
    printf("sort_start\n");

    output_info sequences[200];
    // sequences = (output_info *)malloc(count);

    FILE *input_file;
    input_file = fopen(file_output, "r");
    if (input_file == NULL)
    {
      fprintf(stderr, "fopen failed: input_file\n");
      exit(EXIT_FAILURE);
    }
    // while (count < 200 && fscanf(input_file,
    //               "Query sequence: %s\nDatabase sequence: %s\nBest score: %d\nQ: %s\nD: %s\n\n",
    //               sequences[count].query_sequence,
    //               sequences[count].database_sequence,
    //               &sequences[count].bestScore,
    //               sequences[count].q,
    //               sequences[count].d) != EOF)
    // {
    //   count++;
    //   // sequences = (output_info *)realloc_e(sequences, count * sizeof(output_info), "sequences");
    // }
    fclose(input_file);

    // printf("end\n");
    // printf("query = %d\n", sequences[1].bestScore);
    // qsort(sequences, count)

    // free(sequences);
  }

  MPI_Finalize();

  return 0;
}
