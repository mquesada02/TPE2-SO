#ifndef LIB_H
#define LIB_H

#include <stdint.h>

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);
void numToStr(int64_t num, int base, char * buffer);
void waitForEnter();
void strcopy(char * dest, char * src, int destSize);
int strcmplen(char * str1, char * str2, int len);

char *cpuVendor(char *result);

#endif