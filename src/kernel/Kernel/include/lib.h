#ifndef LIB_H
#define LIB_H

#include <stdint.h>

#define MAX_FDS 24
#define FD_SIZE 2048
void initFDs();

void * memset(void * destination, int32_t character, uint64_t length);
void * memcpy(void * destination, const void * source, uint64_t length);
void numToStr(int64_t num, int base, char * buffer);
void waitForEnter();
void strcopy(char * dest, char * src, int destSize);
int strcmplen(char * str1, char * str2, int len);
unsigned char read(int fd);
void write(int fd, unsigned char c, int FGColor, int BGColor);
void sendEOF(int fd);
void writeFD(int fd, const char *buff);
void writeByteFD(int fd, char c);
char *cpuVendor(char *result);
void wait(int seconds);
uint32_t GetUniform(uint32_t max);
int64_t satoi(char *str);

#endif