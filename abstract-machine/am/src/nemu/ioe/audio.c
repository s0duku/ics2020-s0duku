#include <am.h>
#include <nemu.h>
#include "stdio.h"



#define AUDIO_FREQ_ADDR      (AUDIO_ADDR + 0x00)
#define AUDIO_CHANNELS_ADDR  (AUDIO_ADDR + 0x04)
#define AUDIO_SAMPLES_ADDR   (AUDIO_ADDR + 0x08)
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c)
#define AUDIO_INIT_ADDR      (AUDIO_ADDR + 0x10)
#define AUDIO_COUNT_ADDR     (AUDIO_ADDR + 0x14)

typedef struct am_audio_context{
  uint8_t * reg_write;
}am_audio_context;

static am_audio_context amactx = {(uint8_t *)AUDIO_SBUF_ADDR};

void __am_audio_init(){
  return;
}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg) {
  cfg->present = false;
}

void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl) {
  outl(AUDIO_FREQ_ADDR,ctrl->freq);
  outl(AUDIO_CHANNELS_ADDR,ctrl->channels);
  outl(AUDIO_SAMPLES_ADDR,ctrl->samples);
  outl(AUDIO_SBUF_SIZE_ADDR,0);
  outl(AUDIO_INIT_ADDR,1);
  
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat) {
  stat->count = inl(AUDIO_COUNT_ADDR);
}

void audio_copy_byte(uint8_t *src,uint32_t len){
   uint32_t bs = inl(AUDIO_SBUF_SIZE_ADDR);
    for (uint32_t i = 0;i < len;i++){
      if (amactx.reg_write  >= (bs + (uint8_t *)AUDIO_SBUF_ADDR)){
         amactx.reg_write = (uint8_t *)AUDIO_SBUF_ADDR;
      }
    *amactx.reg_write = src[i];
    amactx.reg_write++;
  }
}

void __am_audio_play(AM_AUDIO_PLAY_T *ctl) {
  audio_copy_byte(ctl->buf.start,ctl->buf.end-ctl->buf.start);
}
