#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

static int cmd_strip(char * cmd){
  size_t len = strlen(cmd);
  int idx=len-1;
  for(;idx >= 0;idx--){
    if(cmd[idx] == '\n' || cmd[idx] == ' ' || cmd[idx] == '\t'){
      cmd[idx] = 0;
    }else{
      break;
    }
  }
  if (idx < 0 ){
    return 0;
  }
  return 1;
}

static void sh_handle_cmd(const char *cmd) {
  if(!cmd_strip((char *)cmd)){
    return;
  }
  execve(cmd,NULL,NULL);
}

void builtin_sh_run() {
  sh_banner();
  sh_prompt();

  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}
