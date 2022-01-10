#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  size_t res = -1;
  for(uint32_t i=0;i<len;i++){
    putch(((char *)buf)[i]);
  }
  res = len;
  return res;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if (ev.keycode == AM_KEY_NONE) return 0;
  return snprintf(buf, len, "%s %s", ev.keydown ? "kd" : "ku", keyname[ev.keycode]);
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T cfg;
  ioe_read(AM_GPU_CONFIG, &cfg);
  return sprintf(buf, "WIDTH : %d\nHEIGHT:%d", cfg.width, cfg.height);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T cfg;
  ioe_read(AM_GPU_CONFIG, &cfg);
  uint32_t offset_x = offset / sizeof(uint32_t) % cfg.width;
  uint32_t offset_y = offset / sizeof(uint32_t) / cfg.width;

  offset = 0;
  while (offset < len / sizeof(uint32_t)) {
    // write pixel by pixel
    AM_GPU_FBDRAW_T ctl = {
      .x = offset_x,
      .y = offset_y,
      .w = 1,
      .h = 1,
      .pixels = (uint32_t*)buf + offset,
      .sync = 1
    };
    ioe_read(AM_GPU_FBDRAW, &ctl);
    offset++;
    offset_x++;
    if (offset_x >= cfg.width) {
      offset_x = 0;
      offset_y++;
    }
  }
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
