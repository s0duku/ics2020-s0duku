#include <common.h>
#include <fs.h>
#include "syscall.h"
#include <proc.h>
#include <am.h>

void * brk_pb(int incre);
void naive_uload(PCB *pcb, const char *filename);

struct timeval {
  long int tv_sec;   /* seconds */
  long int tv_usec;  /* and microseconds */
};

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_brk:
      c->GPRx = (uint32_t)brk_pb(a[1]);break;
    case SYS_yield:
      yield(); c->GPRx = 0;break;
    case SYS_open:
      c->GPRx = fs_open((char *)a[1],a[2],a[3]);break;
    case SYS_close:
      c->GPRx = fs_close(a[1]);break;
    case SYS_read:
      c->GPRx = fs_read(a[1],(void *)a[2],a[3]);break;
    case SYS_write:
      c->GPRx = fs_write(a[1],(void *)a[2],a[3]);break;
    case SYS_lseek:
      c->GPRx = fs_lseek(a[1],a[2],a[3]);break;
    case SYS_gettimeofday:
      ((struct timeval*)a[1])->tv_usec = io_read(AM_TIMER_UPTIME).us;
      ((struct timeval*)a[1])->tv_sec = ((struct timeval*)a[1])->tv_usec / 1000000;
      c->GPRx = 0;break;
    case SYS_execve: 
      c->GPRx = 0;
      naive_uload(NULL,(char*)a[1]);
      break;
    case SYS_exit:
      naive_uload(NULL,"/bin/nterm");break;
    default: 
      panic("Unhandled syscall ID = %d", a[0]);
  }
}
