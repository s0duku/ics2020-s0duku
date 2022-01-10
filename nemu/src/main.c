#include<stdio.h>
#include<stdlib.h>
#include<string.h>
//#include"monitor/debug/expr.h"

//word_t expr(char *e, bool *success);

void init_monitor(int, char *[]);
void engine_start();
int is_exit_status_bad();

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  init_monitor(argc, argv);
  engine_start();

  return is_exit_status_bad();
}
