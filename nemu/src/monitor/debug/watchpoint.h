#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  bool freed;
  char * expr;
  /* TODO: Add more members if necessary */

} WP;
WP * get_wp_list();
WP * get_wp(uint32_t no);
WP * new_wp();
void free_wp(WP *wp);

#endif
