#include <sys/types.h>
static size_t runningPID = 15;

enum state {running, ready, blocked, exited};

void initPriorityQueue();

void addProcess(int priority, unsigned long rsp, size_t pid, char foreground);

void stopProcess(unsigned long rsp, int state);

size_t getRunningPID();

size_t getForegroundPID();

void setProcessStatePQ(size_t pid, int state);

unsigned long selectToRun();

unsigned long schedule(unsigned long rsp);

