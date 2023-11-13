#include <sys/types.h>

#define MAX_PROCESSES 40
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
char getSTDIN(size_t pid);
char getSTDOUT(size_t pid);
void setSTDIN(size_t pid, char stdin);
void setSTDOUT(size_t pid, char stdin);
void waitPID(int pid);
int isBlocked(size_t pid);
int isBlockedFD(size_t pid, int fd);
int isOpenedFD(size_t pid, int fd);
void openFD(size_t pid, int fd);
void closeFD(size_t pid, int fd);
void blockFD(size_t pid, int fd);
void unblockFD(size_t pid, int fd);
unsigned long getRBP(size_t pid);
void blockProcess(size_t pid);
void blockKeyboardProcess(size_t pid);
void blockSemProcess(size_t pid, sem_type * sem, char waiting_locked);
int switchBlock(size_t pid);
int isAlive(size_t pid);
void unblockProcess(size_t pid);
void unblockKeyboardProcess(size_t pid);
int unblockSemProcess(sem_type *sem, char waiting_locked);
void exitProcess();
int killProcess(size_t pid);
int isKBlocked(size_t pid);
int pipeProcess(int priority, void (* process), char argc, char* argv[], char foreground, char* name, char stdin, char stdout);
void loop(char argc, char* argv[]);
void setTestPhilPID(int pid);
int getTestPhilPID();
void createPhil();
void createPhils(int cant);