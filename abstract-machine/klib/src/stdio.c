#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

// void myput(char *buf){
// 	size_t len = strlen(buf);
// 	for(size_t i=0;i<len;i++){
// 		putch(buf[i]);
// 	}
// }



#define MAX_PRINTF_BUF 2048

int printf(const char *fmt, ...) {
  va_list args;
  char buf[MAX_PRINTF_BUF];
	int i;

	va_start(args, fmt);
	i = vsnprintf(buf,MAX_PRINTF_BUF, fmt, args);
	va_end(args);
  for(int start=0;start<i;start++){
    putch(buf[start]);
  }
  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return vsnprintf(out, 0, fmt, ap);
}

int sprintf(char *buf, const char *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf(buf,0, fmt, args);
	va_end(args);

	return i;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {

  va_list args;
	int i;

	va_start(args, fmt);
	i = vsnprintf(out,n, fmt, args);
	va_end(args);

	return i;
}

static char *simple_itoa(int n,int radix){  
  char *p; 
  register int minus; 
  static char buf[36];
  p = &buf[36]; 
  *--p = '\0'; 
  if (n < 0) { minus = 1; n = -n; } else minus = 0; 
  if (n == 0)  *--p = '0'; 
  else while (n > 0) { *--p = "0123456789abcdef"[n - (n / radix)*radix]; n /= radix; } 
  if (minus) *--p = '-';
  return p;
}

int vsnprintf(char *buf,size_t size,const char *fmt, va_list args)
{
	char * str;
  char *s;
  char *tmp = 0;
  char stop = 0;
  size_t len = 0;
  size = size-1;
  for (str=buf;*fmt;++fmt){
    if (*fmt != '%') {
			*str++ = *fmt;
			continue;
		}
    ++fmt;
    switch (*fmt){
      case 'c':
        *str++ = (unsigned char) va_arg(args, int);
        if((str-buf) > size){stop = 1;break;}
        break;
      case 's':
        s = va_arg(args, char *);
        len = strlen(s);
        for(size_t i = 0;i < len;i++){
          *str++ = s[i];
          if((str-buf) > size){stop = 1;break;}
        }
        break;
      case 'd':
        tmp = simple_itoa(va_arg(args, unsigned long),10);
        // myput(tmp);
        len = strlen(tmp);
        for(size_t i = 0;i < len;i++){
          *str++ = tmp[i];
          if((str-buf) > size){stop = 1;break;}
        }
        break;

    }

    if (stop){
      break;
    }
  }
  *str = 0;
	return str-buf;//返回字符个数
}

#endif
