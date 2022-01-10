#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include <rtl/rtl.h>
#include "reg.h"

 //这里的指令都是ISA相关的所以直接访问cpu即可
 //具体实现取决于不同 ISA，如x86的push会用到esp寄存器
/* RTL pseudo instructions */

//从寄存器中取值
static inline def_rtl(lr, rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_mv(s, dest, &reg_l(r)); return;
    case 1: rtl_host_lm(s, dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(s, dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

//存储值到寄存器
static inline def_rtl(sr, int r, const rtlreg_t* src1, int width) {
  switch (width) {
    case 4: rtl_mv(s, &reg_l(r), src1); return;
    case 1: rtl_host_sm(s, &reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(s, &reg_w(r), src1, 2); return;
    default: assert(0);
  }
}

// 32 位压栈 
static inline def_rtl(push, const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  //我们只实现 32 位版本
  assert(s->isa.is_operand_size_16 != true);
  // fix a bug, this tmp is important for example push esp
  if(src1 == &cpu.esp){
    rtl_sm(s,&cpu.esp,-4,src1,4);
    cpu.esp-=4;
  }else{
    cpu.esp -= 4;
    rtl_sm(s,&cpu.esp,0,src1,4);
  }
}

//32 位出栈
static inline def_rtl(pop, rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4
  rtl_lm(s,dest,&cpu.esp,0,4); //fix bug
  cpu.esp += 4;
}

static inline def_rtl(is_sub_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
  rtl_msb(s,t0,src1,width);
  rtl_msb(s,t1,src2,width);
  rtl_msb(s,dest,res,width);
  if(*t0 != *t1 && *t0 != *dest)
    *dest = 1;
  else *dest = 0;
}

// sub 是否产生借位
static inline def_rtl(is_sub_carry, rtlreg_t* dest,
    const rtlreg_t* src1, const rtlreg_t* src2) {
  // dest <- is_carry(src1 - src2)
  if(id_dest->width == 1)
  {
    if( (uint8_t)*src1 <  (uint8_t) *src2)
      *dest = 1;
    else *dest = 0;
  }
  else if(s->isa.is_operand_size_16)
  {
    if( (uint16_t)*src1 <  (uint16_t) *src2)
      *dest = 1;
    else *dest = 0;
  }
  else {
    if( (uint32_t) *src1  < (uint32_t) *src2)
      *dest = 1;
    else *dest = 0;
  }
}

static inline def_rtl(is_add_overflow, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
  rtl_msb(s,t0,src1,width);
  rtl_msb(s,t1,src2,width);
  rtl_msb(s,dest,res,width);
  //如果连个数字最高位相同，res结果有效位却不同那就是溢出
  if(*t0 == *t1 && *dest != *t1)
    *dest = 1;
  else *dest = 0;
}

//是否产生进位
static inline def_rtl(is_add_carry, rtlreg_t* dest,
    const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 + src2)
  if(id_dest->width == 1)
  {
    if((uint8_t)*res < (uint8_t)*src1)
      *dest = 1;
    else *dest = 0;
  }else if(s->isa.is_operand_size_16)
  {
    if( (uint16_t)*res < (uint16_t)*src1 )
      *dest = 1;
    else *dest = 0;
  }
  else{
    if( (uint32_t)*res < (uint32_t)*src1 )
      *dest = 1;
    else *dest = 0;
  }
}

//标志位设置
#define def_rtl_setget_eflags(f) \
  static inline def_rtl(concat(set_, f), const rtlreg_t* src) { \
    cpu.eflags.f = *src;\
  } \
  static inline def_rtl(concat(get_, f), rtlreg_t* dest) { \
    *dest = cpu.eflags.f;\
  }

def_rtl_setget_eflags(CF)
def_rtl_setget_eflags(OF)
def_rtl_setget_eflags(ZF)
def_rtl_setget_eflags(SF)

//判断对应宽度是否为0
static inline def_rtl(update_ZF, const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  switch (width)
  {
  case 1:*t1 = 0xff; break;
  case 2:*t1 = 0xffff; break;
  case 4:*t1 = 0xffffffff; break;
  }
  rtl_and(s,t0,result,t1);
  cpu.eflags.ZF = *t0 == 0;
}

//直接判断最高有效位，决定是否有符号
static inline def_rtl(update_SF, const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  rtl_msb(s,t0,result,width);
  cpu.eflags.SF = *t0 == 1;
}

static inline def_rtl(update_ZFSF, const rtlreg_t* result, int width) {
  rtl_update_ZF(s, result, width);
  rtl_update_SF(s, result, width);
}

#endif
