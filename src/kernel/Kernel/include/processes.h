#include <sys/types.h>

void initProcesses();
void startProcess(int priority, void (* process), char argc, char* argv[], char foreground);
unsigned long prepareHalt();
void setProcessState(size_t pid, int state);
int getProcessState(size_t pid);
void blockProcess(size_t pid);
void unblockProcess(size_t pid);
void exitProcess();