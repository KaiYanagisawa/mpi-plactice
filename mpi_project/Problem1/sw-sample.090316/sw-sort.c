#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <mpi.h>

#define LINE_MAX 2048

typedef struct
{
  char query_sequence[128];
  char database_sequence[128];
  int bestScore;
  char q[512];
  char d[512];
} output_info;

int compare_sequences(const output_info *a, const output_info *b)
{
}

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

    output_info *sequences;
    sequences = (output_info *)malloc(1 * sizeof(output_info));

    FILE *input_file;
    input_file = fopen(file_output, "r");
    if (input_file == NULL)
    {
      fprintf(stderr, "fopen failed: input_file\n");
      exit(EXIT_FAILURE);
    }
    while (fgets(line, LINE_MAX, input_file) != NULL)
    {
      sscanf(line, "Query sequence: %s", sequences[count].query_sequence);
      fgets(line, LINE_MAX, input_file);
      sscanf(line, "Database sequence: %s", sequences[count].database_sequence);
      fgets(line, LINE_MAX, input_file);
      sscanf(line, "Best score: %d", &sequences[count].bestScore);
      fgets(line, LINE_MAX, input_file);
      sscanf(line, "Q: %s", sequences[count].q);
      fgets(line, LINE_MAX, input_file);
      sscanf(line, "D: %s", sequences[count].d);
      fgets(line, LINE_MAX, input_file);

      count++;

      sequences = (output_info *)realloc(sequences, count * sizeof(output_info));
    }
    fclose(input_file);

    qsort(sequences, count, sizeof(output_info), compare_sequences);

    free(sequences);
  }

  MPI_Finalize();

  return 0;
}