#include <stdio.h>

#define REG 0x1
#define POINTER (int *)REG

int main() {
  int a = 1;
  POINTER = &a;
  printf("Value at REG: %d\n", *(int *)REG);
}