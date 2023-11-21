#include <stdio.h>

union IntAndChar
{
  int a;
  char c[4];
} i_and_c;

int main()
{
  // int a = 0x12345678;

  // i_and_c.a = 100;
  i_and_c.c[0] = 'A';
  i_and_c.c[1] = 'B';
  i_and_c.c[2] = 'C';
  i_and_c.c[3] = '\0';
  i_and_c.a = 0x0102;
  if (i_and_c.c[0] == 0x01 && i_and_c.c[1] == 0x02)
    printf("big");
  else if (i_and_c.c[0] == 0x02 && i_and_c.c[1] == 0x01)
    printf("little");
  else
    printf("unknown");
  
  printf("%X\n", &i_and_c);
  printf("%X\n", i_and_c.a);
  // printf("%X\n", i_and_c.c[0]);
  // printf("%X\n", i_and_c.c[1]);
  // printf("%X\n", i_and_c.c[2]);
  // printf("%X\n", i_and_c.c[3]);
  printf("%X %X %X %X", i_and_c.c[0], i_and_c.c[1], i_and_c.c[2], i_and_c.c[3]);
}
