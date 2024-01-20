#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>

#define LINE_MAX 2048

typedef struct
{
  int query_id;
  char query_name[64];
  int query_length;

  int database_id;
  char database_name[64];
  int database_length;

  int best_score;

  int q_start;
  char q[512];
  int q_end;

  int d_start;
  char d[512];
  int d_end;
} output_info;

int compare_sequences(const void *a, const void *b)
{
  if (((output_info *)a)->query_id > ((output_info *)b)->query_id)
  {
    return 1;
  }
  else if (((output_info *)a)->query_id < ((output_info *)b)->query_id)
  {
    return -1;
  }
  else
  {
    if (((output_info *)a)->database_id > ((output_info *)b)->database_id)
    {
      return 1;
    }
    else if (((output_info *)a)->database_id < ((output_info *)b)->database_id)
    {
      return -1;
    }
    else
    {
      return 0;
    }
  }
}

int main(int argc, char **argv)
{
  char *file_output;

  assert(argc >= 2);
  file_output = argv[1];

  char line[LINE_MAX];
  int count;
  int i;

  output_info *sequences;
  sequences = (output_info *)malloc(1 * sizeof(output_info));

  FILE *input_file, *sort_file;

  input_file = fopen(file_output, "r");
  if (input_file == NULL)
  {
    fprintf(stderr, "fopen failed: input_file\n");
    exit(EXIT_FAILURE);
  }

  count = 0;
  while (fgets(line, LINE_MAX, input_file) != NULL)
  {
    sscanf(line,
           "Query sequence: %d_%[^_]_%d",
           &sequences[count].query_id,
           sequences[count].query_name,
           &sequences[count].query_length);
    fgets(line, LINE_MAX, input_file);
    sscanf(line,
           "Database sequence: %d_%[^_]_%d",
           &sequences[count].database_id,
           sequences[count].database_name,
           &sequences[count].database_length);
    fgets(line, LINE_MAX, input_file);
    sscanf(line, "Best score: %d", &sequences[count].best_score);
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

  sort_file = fopen(file_output, "w");
  if (sort_file == NULL)
  {
    perror("Error opening sort_file");
    exit(EXIT_FAILURE);
  }

  for (i = 0; i < count; i++)
  {
    fprintf(sort_file,
            "Query sequence: %d_%s_%d\n",
            sequences[i].query_id,
            sequences[i].query_name,
            sequences[i].query_length);
    fprintf(sort_file,
            "Database sequence: %d_%s_%d\n",
            sequences[i].database_id,
            sequences[i].database_name,
            sequences[i].database_length);
    fprintf(sort_file, "Best score: %d\n", sequences[i].best_score);
    fprintf(sort_file, "Q: %s\n", sequences[i].q);
    fprintf(sort_file, "D: %s\n\n", sequences[i].d);
  }
  fclose(sort_file);

  free(sequences);

  return 0;
}
