#include "cc.h"

static inline def_EHelper(jmp) {
  // the target address is calculated at the decode stage
  assert(s->isa.is_operand_size_16 != true);
  rtl_j(s, s->jmp_pc);

  print_asm("jmp %x", s->jmp_pc);
}


static inline def_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint32_t cc = s->opcode & 0xf;
  rtl_setcc(s, s0, cc);
  rtl_jrelop(s, RELOP_NE, s0, rz, s->jmp_pc);

  print_asm("j%s %x", get_cc_name(cc), s->jmp_pc);
}

static inline def_EHelper(jmp_rm) {
  assert(s->isa.is_operand_size_16 != true);
  rtl_jr(s, ddest);
  print_asm("jmp *%s", id_dest->str);
}

//暂时只实现32位操作数的版本
static inline def_EHelper(call) {
  // the target address is calculated at the decode stage
  assert(s->isa.is_operand_size_16 != true);
  //printf("Call Now esp %x\n",cpu.esp);
  rtl_push(s,&s->seq_pc);
  rtl_j(s,s->jmp_pc);
  print_asm("call %x", s->jmp_pc);
}

static inline def_EHelper(ret) {
  assert(s->isa.is_operand_size_16 != true);
  rtl_pop(s,&s->jmp_pc);
  rtl_j(s,s->jmp_pc);
  print_asm("ret");
}

static inline def_EHelper(ret_imm) {
  TODO();

  print_asm("ret %s", id_dest->str);
}

static inline def_EHelper(call_rm) {
  assert(s->isa.is_operand_size_16 != true);
  rtl_push(s,&s->seq_pc);
  rtl_jr(s,ddest);
  print_asm("call *%s", id_dest->str);
}


/*
t0, t1, ... - 只能在RTL伪指令的实现过程中存放中间结果
s0, s1, ... - 只能在译码辅助函数和执行辅助函数的实现过程中存放中间结果
*/