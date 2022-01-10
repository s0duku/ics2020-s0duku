#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB, FD_EVENTS, FD_FRAMEBUF};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len) ;
/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write/*invalid_write*/},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write/*invalid_write*/},
  [FD_EVENTS] = {"/dev/events",0,0,events_read, invalid_write},
  [FD_FRAMEBUF] = {"/dev/fb",0,0,dispinfo_read, fb_write},  
#include "files.h"
};

#define TOTAL_FILES_NUM (sizeof(file_table) / sizeof(Finfo))

#define FD_CHECK(x) if(fd < 0 || fd >= x){panic("Invalid fd");}

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t get_ramdisk_size();



void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char *pathname, int flags, int mode){
  for (size_t i = 0;i < TOTAL_FILES_NUM;i++){
    if(!strcmp(pathname,file_table[i].name)){
      file_table[i].open_offset = 0;
      return i;
    }
  }
  assert(false);
  return -1;
}

int fs_close(int fd){
  return 0;
}

size_t fs_read(int fd, void *buf, size_t len){
  FD_CHECK(TOTAL_FILES_NUM)
  size_t slen = len;
  if (file_table[fd].open_offset + len > file_table[fd].size) {
    if (file_table[fd].size < file_table[fd].open_offset) len = 0;
    else len = file_table[fd].size - file_table[fd].open_offset;
  }
  size_t res = 0;
  size_t offset = file_table[fd].disk_offset + file_table[fd].open_offset;
  if(file_table[fd].read){
    res = file_table[fd].read(buf,offset,slen);
  }else{
    res = ramdisk_read(buf, offset, len);
  }
  file_table[fd].open_offset += res;
  return res;
}
size_t fs_write(int fd, const void *buf, size_t len){
  FD_CHECK(TOTAL_FILES_NUM)
  size_t slen = len;
  if (file_table[fd].open_offset + len > file_table[fd].size) {
    if (file_table[fd].size < file_table[fd].open_offset) len = 0;
    else len = file_table[fd].size - file_table[fd].open_offset;
  }
  size_t res = 0;
  size_t offset = file_table[fd].disk_offset + file_table[fd].open_offset;
  if(file_table[fd].write){
    //printf(buf);
    res = file_table[fd].write(buf,offset,slen);
  }else{
    res = ramdisk_write(buf, offset, len);
  }
  file_table[fd].open_offset += res;
  return res;
}
size_t fs_lseek(int fd, size_t offset, int whence){
  FD_CHECK(TOTAL_FILES_NUM)
  switch (whence){
    case SEEK_SET:
      file_table[fd].open_offset = offset;break;
    case SEEK_CUR:
      file_table[fd].open_offset += offset;break;
    case SEEK_END:
      file_table[fd].open_offset += file_table[fd].size + offset;break;
  default:
    panic("Invalid parameter for fs_lseek");
  }
  return file_table[fd].open_offset;
}