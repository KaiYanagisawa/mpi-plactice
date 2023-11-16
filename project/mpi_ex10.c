/*
   演習2 (データアラインメイトの確認)
   char, int, double, floatの4つのデータ型の変数をそれぞれ2個以上ずつ宣言する
   その宣言の順序をいろいろと入れ替えた時にそれぞれの変数のアドレスを確認して、変数がメモリ内に配置される規>則性を見つけなさい。
   ただし、アドレスはprintf("変数名: c1: %X\n", &c1);で表示できる
*/
#include <stdio.h>

int main(int argc, char *argv[]) {
  // int i1;
  // int i2;
  // double d1;
  // double d2;
  // char c1;
  // char c2;
  // float f1;
  // float f2;

  int i1;
  int i2;
  char c1;
  char c2;
  double d1;
  double d2;
  float f1;
  float f2;

  // int i1;
  // int i2;
  // float f1;
  // float f2;
  // double d1;
  // double d2;
  // char c1;
  // char c2;

  printf("変数名: c1: %X\n", &c1);
  printf("変数名: c2: %X\n", &c2);
  printf("変数名: i1: %X\n", &i1);
  printf("変数名: i2: %X\n", &i2);
  printf("変数名: d1: %X\n", &d1);
  printf("変数名: d2: %X\n", &d2);
  printf("変数名: f1: %X\n", &f1);
  printf("変数名: f2: %X\n", &f2);

  return 0;
}