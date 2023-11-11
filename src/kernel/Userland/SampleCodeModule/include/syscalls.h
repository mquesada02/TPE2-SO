#ifndef _SYSCALLS_H_
#define _SYSCALLS_H_

#include <stdint.h>
#include <sys/types.h>

struct processStart {
    char foreground;
    char * name;
};
typedef int sem_type;

extern unsigned char syscall_read(int fd);
extern long int * syscall_getRegisters();
extern void syscall_writeAt(unsigned char c, int x, int y, int FGColor, int BGColor);
extern void syscall_write(unsigned char c, int FGColor, int BGColor);
extern void syscall_nextLine();
extern void syscall_wait(int seconds);
extern void syscall_beep(uint32_t freq, uint32_t time);
extern void syscall_loadScreen();
extern void syscall_enableDoubleBuffer(int enable);
extern void syscall_time(char * timeStr);
extern void syscall_getKeyboardState(char * keys);
extern void syscall_drawCircle(int x_centre, int y_centre, int r, uint8_t color);
extern void syscall_drawRectangle(int x, int y, int h, int w, uint8_t color);
extern void * syscall_allocMemory(size_t size);
extern int syscall_freeMemory(void *data);
extern int syscall_getMemStatus(size_t * free, size_t * occupied);
extern int syscall_startProcess(int priority, void (* process), char argc, char* argv[], struct processStart * ps);
extern void syscall_exit();
extern size_t syscall_getpid();
extern int syscall_kill(size_t pid);
extern void syscall_ps();
extern int syscall_switchBlock(size_t pid);
extern int syscall_changePriority(size_t pid, int priority);
extern void syscall_waitpid(int pid);
extern sem_type * syscall_sem_open(char * name, int value);
extern int syscall_sem_close(sem_type * sem);
extern int syscall_sem_wait(sem_type * sem);
extern int syscall_sem_post(sem_type * sem);

#endif