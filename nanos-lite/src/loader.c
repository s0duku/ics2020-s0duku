#include <proc.h>
#include <elf.h>
#include <fs.h>



#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len);

// 把`buf`中的`len`字节写入到ramdisk中`offset`偏移处
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

// 返回ramdisk的大小, 单位为字节
size_t get_ramdisk_size();

#define RAMDISK_ELF_OFFSET 0

static uintptr_t loader(PCB *pcb, const char *filename) {
  
  Elf_Ehdr elfhdr;
  Elf_Phdr prohdr;

  int elf_fd = -1;
  elf_fd = fs_open(filename,0,0);
  assert(elf_fd > 2);
  fs_read(elf_fd,&elfhdr,sizeof(Elf_Ehdr));
  
  for (int i = 0;i < elfhdr.e_phnum;i++){
    fs_lseek(elf_fd, elfhdr.e_phoff + i * sizeof(Elf_Phdr), SEEK_SET);
    fs_read(elf_fd,&prohdr,sizeof(Elf_Phdr));
    void *vaddr = (void *)(prohdr.p_vaddr);
    size_t vsize = prohdr.p_filesz;
    void *file_vaddr = (void*)(prohdr.p_vaddr + prohdr.p_filesz);
    void *mem_vaddr = (void*)(prohdr.p_vaddr + prohdr.p_memsz);
    fs_lseek(elf_fd, prohdr.p_offset, SEEK_SET);
    fs_read(elf_fd,vaddr,vsize);
    if (mem_vaddr>file_vaddr)
      memset(file_vaddr,0,mem_vaddr-file_vaddr);
    else
      memset(file_vaddr,0,file_vaddr-mem_vaddr);
  }



  // assert(get_ramdisk_size() != 0);
  // size_t read_offset = RAMDISK_ELF_OFFSET;
  // ramdisk_read(&elfhdr, read_offset,sizeof(Elf_Ehdr));
  // for (int i = 0;i < elfhdr.e_phnum;i++){
  //   read_offset = RAMDISK_ELF_OFFSET + elfhdr.e_phoff + i * sizeof(Elf_Phdr);
  //   ramdisk_read(&prohdr,read_offset,sizeof(Elf_Phdr));
  //   void *vaddr = (void *)(prohdr.p_vaddr);
  //   size_t vsize = prohdr.p_filesz;
  //   void *file_vaddr = (void*)(prohdr.p_vaddr + prohdr.p_filesz);
  //   void *mem_vaddr = (void*)(prohdr.p_vaddr + prohdr.p_memsz);
  //   read_offset = RAMDISK_ELF_OFFSET + prohdr.p_offset;
  //   ramdisk_read(vaddr,read_offset,vsize);
  //   if (mem_vaddr>file_vaddr)
  //     memset(file_vaddr,0,mem_vaddr-file_vaddr);
  //   else
  //     memset(file_vaddr,0,file_vaddr-mem_vaddr);
  // }

  return elfhdr.e_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

