#include <isa.h>
#include "expr.h"
#include "watchpoint.h"
#include <memory/paddr.h>

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
int is_batch_mode();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}


static int cmd_si(char *args) {
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {
    cpu_exec(1);
  }else{
    cpu_exec(strtoull(arg,0,0));
  }
  return 0;
}

static int cmd_x(char *args) {
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {
    printf("No arguments \n");
  }else{
    char *expr = strtok(NULL, " ");
    if (expr == NULL){
      printf("No arguments \n");
    }else {
      uint64_t n = strtoull(arg,0,0);
      uint64_t gaddr = strtoull(expr,0,0);
      for (int i = 0;i < n;i++){
        // fix bug, the reason we doing gaddr-PMEM_BASE beacasue different isa has different address encode
        // this addr provide by user, we need to modify it
        printf("0x%08lX:\t0x%08X\n",gaddr,*((uint32_t *)guest_to_host(gaddr-PMEM_BASE)));
        gaddr += 4;
      }
    }
  }
  return 0;
}

static int cmd_w(char *args) {
  // command for set watchpoint
  if (args == NULL){
    printf("No arguments \n");
    return 0;
  }
  WP *wp = new_wp();
  assert(wp != NULL);
  assert(wp->expr == NULL);
  wp->expr = (char *)malloc(strlen(args));
  memcpy(wp->expr,args,strlen(args));
  printf("Set Watchpoint %d\n",wp->NO);
  return 0;
}

static int cmd_d(char *args){
  // command for delete watchpoint
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {
    printf("No arguments \n");
    return 0;
  }else{
    free_wp(get_wp(strtoul(arg,NULL,0)));
  }
  return 0;
}

static int cmd_e(char *args) {
  // command for test expression
  bool success = true;
  word_t res = 0;
  // fix bug about check args, we cant use strtok to check args, it will split the expression
  // for example 1   +1, will be spilt into 1
  if (args == NULL){
    printf("No arguments \n");
    return 0;
  }
  res = expr(args,&success);
  if (success){
    printf("Expression value: %X\n",res);
  }
  return 0;
}

static int cmd_info(char *args) {
  // command for show information
  char *arg = strtok(args, " ");
  if (arg == NULL) {
    printf("No arguments \n");
  }else{
    if(strcmp(arg,"r") == 0){
      isa_reg_display();
    }else if(strcmp(arg,"w") == 0){
      WP *wpl = get_wp_list();
      while(wpl){
        printf("watchpoint %02d:\t%s\n",wpl->NO,wpl->expr);
        wpl = wpl->next;
      }
    }else{
      printf("Unknown command '%s'\n", arg);
    }
  }
  return 0;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Step N Instructions", cmd_si },
  { "info", "Print Infomation\n\tr\treg value\n\tw\twatchpoint", cmd_info },
  { "x", "Check N Dword Memories", cmd_x },
  { "e", "Test Expression", cmd_e },
  { "w", "Set Watchpoint", cmd_w },
  { "d", "Delete Watchpoint", cmd_d },

  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop() {
  if (is_batch_mode()) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
