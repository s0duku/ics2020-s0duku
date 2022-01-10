#include <common.h>

#ifdef HAS_IOE

#include <device/map.h>
#include <SDL2/SDL.h>

#define AUDIO_PORT 0x200 // Note that this is not the standard
#define AUDIO_MMIO 0xa1000200
#define STREAM_BUF 0xa0800000
#define STREAM_BUF_MAX_SIZE 65536*64

enum {
  reg_freq,
  reg_channels,
  reg_samples,
  reg_sbuf_size,
  reg_init,
  reg_count,
  nr_reg
};

typedef struct audio_context{
  SDL_AudioSpec want;
  SDL_mutex * reg_count_mutex;
  uint32_t reg_count;
  uint8_t * reg_read;
  uint32_t sbuf_size;


}audio_context;

static uint8_t *sbuf = NULL;
static uint32_t *audio_base = NULL;

static audio_context actx;

SDL_AudioSpec want, have;

static inline void audio_play(void *userdata, uint8_t *stream, int len) {
  SDL_LockMutex(actx.reg_count_mutex);
  //printf("read at %p ",actx.reg_read);
  if (len > actx.reg_count){
    // remove noise
    memset(stream + actx.reg_count,0,sizeof(uint8_t)*(len - actx.reg_count));
    len = actx.reg_count;
  }
  //  printf("reg count %d ",actx.reg_count);
  //  printf("read len %d ",len);
  //  printf("\n");

  for (uint32_t i = 0;i < len;i++){
    if (actx.reg_read  >= actx.sbuf_size + sbuf){
      actx.reg_read = sbuf;
    }
    stream[i] = *actx.reg_read;
    *actx.reg_read = 0;
    actx.reg_read++;
  }
  printf("stream %u\n",stream[0]);
  actx.reg_count -= len;
  if(actx.reg_count == 0){
    SDL_PauseAudio(-1);
  }
  SDL_UnlockMutex(actx.reg_count_mutex);
  // if(actx.reg_count == 0){
  //   printf("PCM Stream Buff Run Out %d, Read Ptr %p\n",actx.reg_count,actx.reg_read);
  // }
}

static void audio_io_handler(uint32_t offset, int len, bool is_write) {
  // attention about the value size !!!
  assert(offset % 4 == 0);
  uint32_t idx = offset / 4;
  switch (idx){
    case reg_freq:
      if(is_write){
        actx.want.freq = audio_base[reg_freq];
        printf("Set Freq %d\n",actx.want.freq);
      }
      audio_base[reg_freq] = actx.want.freq;
      break;
    case reg_channels:
      if(is_write){
        actx.want.channels = audio_base[reg_channels];
        printf("Set Channles %d\n",actx.want.channels);
      }
      audio_base[reg_channels] = actx.want.channels;
      break;
    case reg_samples:
      if(is_write){
        actx.want.samples = audio_base[reg_samples];
        printf("Set Samples %d\n",actx.want.samples);
      }
      audio_base[reg_samples] = actx.want.samples;
      break;
    case reg_sbuf_size:
      if (is_write){
        assert(audio_base[reg_sbuf_size]<=STREAM_BUF_MAX_SIZE);
        actx.sbuf_size = audio_base[reg_sbuf_size] == 0 ? STREAM_BUF_MAX_SIZE:audio_base[reg_sbuf_size];
        printf("Set Sbuf Size %d\n",actx.sbuf_size);
      }
      audio_base[reg_sbuf_size] = actx.sbuf_size;
      break;
    case reg_init:
      SDL_InitSubSystem(SDL_INIT_AUDIO);SDL_OpenAudio(&actx.want, NULL);SDL_PauseAudio(0);break;
    case reg_count:
      SDL_LockMutex(actx.reg_count_mutex);
      audio_base[reg_count] = actx.reg_count;
      SDL_UnlockMutex(actx.reg_count_mutex);break;
    default:
      assert(false);
  }
}

static void audio_sbuf_handler(uint32_t offset, int len, bool is_write) {
  if (is_write){
    //printf("sbuf write at %p, value %x, len %d \n",sbuf+offset,*(sbuf+offset),len);
    SDL_LockMutex(actx.reg_count_mutex);
    actx.reg_count += len;
    SDL_UnlockMutex(actx.reg_count_mutex);
    SDL_PauseAudio(0);
  }
}


static void init_audiocontext(){
  memset(&actx.want,0,sizeof(actx.want));
  actx.reg_count_mutex = SDL_CreateMutex();
  actx.want.format = AUDIO_S16SYS;
  actx.want.userdata = NULL;
  actx.want.callback = audio_play;
  actx.sbuf_size = STREAM_BUF_MAX_SIZE;
  actx.reg_count = 0;
  actx.reg_read = sbuf;
}

void init_audio() {
  uint32_t space_size = sizeof(uint32_t) * nr_reg;
  audio_base = (void *)new_space(space_size);
  add_pio_map("audio", AUDIO_PORT, (void *)audio_base, space_size, audio_io_handler);
  add_mmio_map("audio", AUDIO_MMIO, (void *)audio_base, space_size, audio_io_handler);

  sbuf = (void *)new_space(STREAM_BUF_MAX_SIZE);
  init_audiocontext();
  add_mmio_map("audio-sbuf", STREAM_BUF, (void *)sbuf, STREAM_BUF_MAX_SIZE, audio_sbuf_handler);
}
#endif	/* HAS_IOE */
