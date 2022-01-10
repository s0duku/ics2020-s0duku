#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  char * p = (char *)s;
  while(*p){
    len++;
    p++;
  }
  return len;
}

char *strcpy(char* dst,const char* src) {
  char *p = (char *)src;
  char *dt = dst;
  while(*p){
    *dt = *p;
    dt++;
    p++;
  }
  *dt = 0;
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  size_t i = 0;
  for (;i<n;i++){
    dst[i] = src[i];
  }
  dst[i] = 0;
  return dst;
}

char* strcat(char* dst, const char* src) {
  size_t pos = strlen(dst);
  char *p = (char *)src;
  while(*p){
    dst[pos++] = *p;
    p++;
  }
  dst[pos] = 0;
  return dst;
}

int strcmp(const char* s1, const char* s2) {
  while(*s1 && (*s1==*s2)){
        ++s1;
        ++s2;
    }
  return *s1 - *s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
  for (int i = 0; i < n; ++i) {
		if (*(s1 + i) > *(s2 + i)) {
			return 1;
		}
		else if (*(s1 + i) < *(s2 + i)) {
			return -1;
		}
		if (*(s1 + i) == 0 || *(s2 + i) == 0) {
			break;
		}
	}
	return 0;
}

void* memset(void* v,int c,size_t n) {
  if (NULL == v || n < 0)
		return NULL;
	char * tmpS = (char *)v;
	while(n-- > 0)
		*tmpS++ = c;
	return v;
}

void* memmove(void* dst,const void* src,size_t n) {
  assert(NULL !=src && NULL !=dst);
    char* tmpdst = (char*)dst;
    char* tmpsrc = (char*)src;

    if (tmpdst <= tmpsrc || tmpdst >= tmpsrc + n)
    {
        while(n--){
            *tmpdst++ = *tmpsrc++; 
        }
    }
    else
    {
        tmpdst = tmpdst + n - 1;
        tmpsrc = tmpsrc + n - 1;
        while(n--){
            *tmpdst-- = *tmpsrc--;
        }
    }
    return dst;
}

void* memcpy(void* out, const void* in, size_t n) {
  if (NULL == out || NULL == in || n < 0)
		return NULL;
	char *tempDest = (char *)out;
	char *tempSrc = (char *)in;
 
	while (n-- > 0)
		*tempDest++ = *tempSrc++;
	return out;
}

int memcmp(const void* s1, const void* s2, size_t n) {
  const unsigned char *su1, *su2;
    int res = 0;
    for (su1 = s1, su2 = s2; 0 < n; ++su1, ++su2, n--)
        if ((res = *su1 - *su2) != 0)
            break;
    return res;
}

#endif
