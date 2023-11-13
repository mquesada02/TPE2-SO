#include <sys/types.h>
#include <MemoryManager.h>
#include <processes.h>


void initPriorityQueue();

void addProcess(int priority, unsigned long rsp, size_t pid, char foreground);

void stopProcess(unsigned long rsp, int state);

void removeProcess(size_t pid);

void setPriorityCounter();

void printProcesses();

int changePriority(size_t pid, int priority);size_t getRunningPID();



size_t getForegroundPID();

void setForegroundPID(size_t foreground);

unsigned long selectToRun();

unsigned long schedule(unsigned long rsp);

