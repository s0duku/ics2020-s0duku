#include <NDL.h>
#include <SDL.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  char buf[1000] = {0};
  char kd[4] = "kd ";
  if (NDL_PollEvent(buf, 1000)) {
    if (memcmp(buf, kd, sizeof(char) * 3) == 0)
      ev->key.type = SDL_KEYDOWN;
    else
      ev->key.type = SDL_KEYUP;
    char ev_key[10];
    sscanf(buf + 3, "%s", ev_key);
    for (int i = 0; i < 83; i++)
      if (strcmp(keyname[i], ev_key) == 0) {
        ev->key.keysym.sym = i;
      }
    return 1;
  } else {
    return 0;
  }
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  while (1) {
    if (SDL_PollEvent(event)) {
      break;
    }
  }
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
