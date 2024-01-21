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
  int id;
  char name[64];
  int length;
} output_info;

int main(int argc, char **argv)
{
  char *file_output;

  assert(argc >= 2);
  file_output = argv[1];

  char line[LINE_MAX];
  int count;
  int i;
  int sequence_length;
  int string_0_500 = 0;
  int string_500_1000 = 0;
  int string_1000_2000 = 0;
  int string_2000_3000 = 0;
  int string_3000_4000 = 0;
  int string_4000_10000 = 0;
  int string_10000 = 0;
  int string_100000 = 0;

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
    if (line[0] == '>')
    {
      sscanf(line,
             ">%d_%[^_]_%d",
             &sequences[count].id,
             sequences[count].name,
             &sequences[count].length);
    }
    // fgets(line, LINE_MAX, input_file);

    count++;

    sequences = (output_info *)realloc(sequences, (count * 2) * sizeof(output_info));
  }
  fclose(input_file);

  for (i = 0; i < count; i++)
  {
    sequence_length = sequences[i].length;

    if (sequence_length > 0 && sequence_length <= 500)
    {
      string_0_500++;
    }
    else if (sequence_length > 500 && sequence_length <= 1000)
    {
      string_500_1000++;
    }
    else if (sequence_length > 1000 && sequence_length <= 2000)
    {
      string_1000_2000++;
    }
    else if (sequence_length > 2000 && sequence_length <= 3000)
    {
      string_2000_3000++;
    }
    else if (sequence_length > 3000 && sequence_length <= 4000)
    {
      string_3000_4000++;
    }
    else if (sequence_length > 4000 && sequence_length <= 10000)
    {
      string_4000_10000++;
    }
    else if (sequence_length > 10000 && sequence_length <= 100000)
    {
      string_10000++;
    }
    else if (sequence_length > 100000)
    {
      string_100000++;
    }
  }

  printf("\t 0_500=%d\n\
          500_1000=%d\n\
          1000_2000=%d\n\
          2000_3000=%d\n\
          3000_4000=%d\n\
          4000_10000=%d\n\
          10000_100000=%d\n\
          100000_=%d",
         string_0_500,
         string_500_1000,
         string_1000_2000,
         string_2000_3000,
         string_3000_4000,
         string_4000_10000,
         string_10000,
         string_100000);

  free(sequences);

  return 0;
}
