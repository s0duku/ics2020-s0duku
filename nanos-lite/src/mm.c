#include <memory.h>

static void *pf = NULL;
static void *pb = NULL;

void* new_page(size_t nr_page) {
  return NULL;
}

static inline void* pg_alloc(int n) {
  return NULL;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  return 0;
}

void init_pb(){
  pb = pf;
}

void * brk_pb(int incre){
  void * res = pb;
  pb += incre;
  return res;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  init_pb();
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
