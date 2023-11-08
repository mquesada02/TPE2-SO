#include <sys/types.h>

#define MAX_PROCESSES 10
#define HALT_PID 0
#define SHELL_PID 1
#define KEYBOARD 1

typedef int sem_type;

void initProcesses();
int startProcess(int priority, void (* process), char argc, char* argv[], char foreground, char* name);
unsigned long prepareHalt();
void setProcessState(size_t pid, int state);
int getProcessState(size_t pid);
char* getName(size_t pid);
unsigned long getRBP(size_t pid);
void blockProcess(size_t pid);
void blockKeyboardProcess(size_t pid);
void blockSemProcess(size_t pid, sem_type * sem);
int switchBlock(size_t pid);
int isAlive(size_t pid);
void unblockProcess(size_t pid);
void unblockKeyboardProcess(size_t pid);
void unblockSemProcess(sem_type *sem);
void blockSemProcess(size_t pid, sem_type * sem);
void exitProcess();
int killProcess(size_t pid);
int isKBlocked(size_t pid);