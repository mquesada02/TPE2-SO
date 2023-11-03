void initPriorityQueue();

void addProcess(int priority, unsigned long rsp);

void stopProcess(unsigned long rsp, int state);

unsigned long selectToRun();

unsigned long schedule(unsigned long rsp);

