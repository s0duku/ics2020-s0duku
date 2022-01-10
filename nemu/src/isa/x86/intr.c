#include <cpu/exec.h>
#include "local-include/rtl.h"

void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t ret_addr)
{
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  //TODO();
  rtl_push(s, &cpu.eflags.eflags_value);
  cpu.eflags.IF = 0;
  cpu.eflags.TF = 0;
  rtl_push(s, &cpu.cs);
  rtl_push(s, &ret_addr);
  uint32_t idt_addr = cpu.idtr.base + 8 * NO;
  uint32_t jmp_pc = vaddr_read(idt_addr, 2) | (vaddr_read(idt_addr + 4, 4) & 0xffff0000);
  rtl_j(s, jmp_pc);  
}

void query_intr(DecodeExecState *s)
{
  assert(cpu.INTR == 0 || cpu.INTR == 1);
  if (cpu.INTR == 1 && cpu.eflags.IF == 1)
  {
    assert(cpu.INTR == 1);
    cpu.INTR = false;
    raise_intr(s, 32, cpu.pc);
    update_pc(s);
  }
}
