#include <processes.h>
#include <scheduler.h>
#include <interrupts.h>
#include <MemoryManager.h>

#define PAGE_SIZE 4096
#define HALT_PAGE 2048

#define MAX_PROCESSES 10

typedef struct process {
    unsigned long rsp;
    size_t pid;
    int state;
    char argc;
    char** argv;
} processType;

processType active_processes[MAX_PROCESSES];
 

extern unsigned long prepare_process(void * memory, void (* process), char argc, char* argv[]);

void initProcesses() {
    for (int i=0;i<MAX_PROCESSES;i++) {
        active_processes[i].rsp = 0;
        active_processes[i].state = ready;
        active_processes[i].pid = 0;
        active_processes[i].argc = 0;
        active_processes[i].argv = NULL;
    }
}

void blockProcess(size_t pid) {
    setProcessState(pid, blocked);
}

void unblockProcess(size_t pid) {
    setProcessState(pid, ready);
}

void setProcessState(size_t pid, int state) {
    if (pid > MAX_PROCESSES) return;
    setProcessStatePQ(pid,state);
    active_processes[pid].state = state;

}

int getProcessState(size_t pid) {
    if (pid > MAX_PROCESSES) return ready;
    return active_processes[pid].state;
}

size_t getFreePID() {
    for (int i=1;i<MAX_PROCESSES;i++) // halt pid = 0
        if (active_processes[i].rsp == 0) 
            return i;
}

void startProcess(int priority, void (* process), char argc, char* argv[], char foreground) {
    _cli();
    void * processMemory = allocMemory(PAGE_SIZE);
    unsigned long rsp = prepare_process(processMemory + PAGE_SIZE, process, argc, argv);
    // int 0x20
    processType newProcess = {rsp, getFreePID(), ready, argc, argv};
    active_processes[newProcess.pid] = newProcess;
    addProcess(priority, rsp, newProcess.pid, foreground);
    _sti();
}

void exitProcess() {
    _cli();
    size_t pid = getRunningPID();
    if (pid == 0 || pid == 15) return;
    active_processes[pid].state = exited;
    setProcessState(pid, exited);
    setProcessState(1,ready); // shell
    while(1) {_hlt();}
    return;
}

void halting() {
    while(1) {
        _hlt();
    }
}

unsigned long prepareHalt() { // pid = 0 is halt
    void * haltMemory = allocMemory(HALT_PAGE);
    unsigned long haltrsp = prepare_process(haltMemory + HALT_PAGE, &halting, 1, (char**){"halt"});
    processType haltProcess = {haltrsp, 0, ready, 1, (char*){"halt"}};
    active_processes[0] = haltProcess;
    return haltrsp;
}


