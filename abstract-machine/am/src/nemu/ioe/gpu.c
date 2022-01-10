#include <am.h>
#include <nemu.h>
#include <string.h>
#define SYNC_ADDR (VGACTL_ADDR + 4)

static uint32_t W;
static uint32_t H;

void __am_gpu_init() {
  H = inw(VGACTL_ADDR);
  W = inw(VGACTL_ADDR+2);
  return;
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = inw(VGACTL_ADDR+2), .height = inw(VGACTL_ADDR),
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  uint32_t *fb = (uint32_t*)FB_ADDR;
  int x = ctl->x, y = ctl->y, w = ctl->w,h = ctl->h;
  uint32_t *pixels = ctl->pixels;
  // High
  for(int i = 0;i < h;i++)
    // Width
    for(int j = 0;j < w;j++){
      if(y+i < H&&x+j <W){
        // fb is pointer to array(array(Width))
        // fb now is uint32 * which is flatten
        // so it is W*(y+i)+x+j
        fb[W*(y+i)+x+j] = pixels[w*i+j];
      }
    }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}

