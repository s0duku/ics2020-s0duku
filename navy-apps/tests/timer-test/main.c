#include <NDL.h>
#include <sys/time.h>
#include <stdio.h>

int main() {
  uint32_t current_msec, last_sec = -1;
  while (1) {
    current_msec = NDL_GetTicks();
    if (current_msec / 1000 != last_sec) {
      last_sec = current_msec / 1000;
      printf("MilliSecond: %d\n", current_msec);
    }
  }
  return 0;
}
