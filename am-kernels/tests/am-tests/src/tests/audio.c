#include <amtest.h>

#define AUDIO_TEST_LOOP

void play_pcm_tilend(void *payload_start,uint32_t payload_len){
  int nplay = 0;
  Area sbuf;
  sbuf.start = payload_start;
  while (nplay < payload_len) {
    int len = (payload_len - nplay > 4096 ? 4096 : payload_len - nplay);
    sbuf.end = sbuf.start + len;
    io_write(AM_AUDIO_PLAY, sbuf);
    sbuf.start += len;
    nplay += len;
    printf("Already play %d/%d bytes of data\n", nplay, payload_len);
  }

  //printf("Wait play to end\n");
  while (io_read(AM_AUDIO_STATUS).count > 0);
}


void audio_test() {
  if (!io_read(AM_AUDIO_CONFIG).present) {
    printf("WARNING: %s does not support audio\n", TOSTRING(__ARCH__));
    return;
  }

  io_write(AM_AUDIO_CTRL, 8000, 1, 1024);

  extern uint8_t audio_payload, audio_payload_end;
  uint32_t audio_len = &audio_payload_end - &audio_payload;
  
  
  #ifdef AUDIO_TEST_LOOP
  while(1)
  #endif
    play_pcm_tilend(&audio_payload,audio_len);
}
