#include <sys/types.h>
#include <processes.h>

static size_t runningPID = MAX_PROCESSES;

enum state {running, ready, blocked, exited};

void initPriorityQueue();

void addProcess(int priority, unsigned long rsp, size_t pid, char foreground);

void stopProcess(unsigned long rsp, int state);

void removeProcess(size_t pid);

void printProcesses();

size_t getRunningPID();

size_t getForegroundPID();

void setForegroundPID(size_t foreground);

unsigned long selectToRun();

unsigned long schedule(unsigned long rsp);

