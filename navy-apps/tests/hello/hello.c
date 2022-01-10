#include <unistd.h>
#include <stdio.h>

int main() {
  write(1, "Hello World!\n", 13);
  printf("Attention! for test sbrk, i modify the hello.c\n");
  printf("Hello World from Navy-apps for the %dth time!\n", 1);
  return 0;
}
