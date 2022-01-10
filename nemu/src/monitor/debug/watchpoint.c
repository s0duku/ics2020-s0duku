#include "watchpoint.h"
#include "expr.h"
#include <stdlib.h>

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].freed = true;
    wp_pool[i].expr = NULL;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].freed = true;
  wp_pool[NR_WP - 1].expr = NULL;
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP * get_wp_list(){
  return head;
}

WP * new_wp() { 
  assert(free_ != NULL);
  WP * res = NULL;
  res = free_;
  free_ = free_->next;
  res->next = head;
  head = res;
  res->freed = false;
  return res;
}

WP * get_wp(uint32_t no){
  return &wp_pool[no];
}

void free_wp(WP *wp) {
  WP *tmp = head;
  if (wp->freed == true){
    return;
  }
  if (tmp == wp){
    head = NULL;
  }
  while(tmp){
    if (tmp->next == wp){
      tmp->next = wp->next;
    }
    tmp = tmp->next;
  }
  free(wp->expr);
  wp->expr = NULL;
  wp->freed = true;
  wp->next = free_;
  free_ = wp;
}