#include <processes.h>
#include <scheduler.h>
#include <interrupts.h>
#include <MemoryManager.h>

#define PAGE_SIZE 4096
#define HALT_PAGE 2048

#define false 0
#define true 1

typedef struct process {
    char* name;
    char alive;
    unsigned long rbp;
    int state;
    char argc;
    char** argv;
    void * dataMemory;
} processType;

processType active_processes[MAX_PROCESSES+1];
char keyboard_blocked_processes[MAX_PROCESSES];
void * sem_blocked_processes[MAX_PROCESSES];
static char i = 0;

extern unsigned long prepare_process(void * memory, void (* process), char argc, char* argv[]);

void initProcesses() {
    for (int i=0;i<MAX_PROCESSES;i++) {
        active_processes[i].alive = false;
        active_processes[i].rbp = 0;
        active_processes[i].state = ready;
        active_processes[i].argc = 0;
        active_processes[i].argv = NULL;
        keyboard_blocked_processes[i] = 0;
        sem_blocked_processes[i] = NULL;
    }
    active_processes[MAX_PROCESSES].alive = false;
    active_processes[MAX_PROCESSES].state = exited;
}

int switchBlock(size_t pid) {
    if(pid <= 1 || pid >= MAX_PROCESSES) return -1;
    int state = getProcessState(pid);
    if (state == blocked) {
        if (active_processes[pid].alive == false)
            return -2;
        else
            unblockProcess(pid);
    } else if (state == ready) {
        if (active_processes[pid].alive == false)
            return -2;
        else
            blockProcess(pid);
    }
    return 0;
}

int isAlive(size_t pid) {
    return active_processes[pid].alive;
}

void blockProcess(size_t pid) {
    setProcessState(pid, blocked);
}

void unblockProcess(size_t pid) {
    setProcessState(pid, ready);
}

void blockKeyboardProcess(size_t pid) {
    if (pid < 0 || pid >= MAX_PROCESSES) return;
    blockProcess(pid);
    keyboard_blocked_processes[pid] = true;
}

void blockSemProcess(size_t pid, sem_type * sem) {
    if (pid < 0 || pid >= MAX_PROCESSES) return;
    blockProcess(pid);
    sem_blocked_processes[pid] = sem;
}

int isKBlocked(size_t pid) {
    return keyboard_blocked_processes[pid];
}

void unblockKeyboardProcess(size_t pid) {
    if (pid < 0 || pid >= MAX_PROCESSES) return;
    unblockProcess(pid);
    keyboard_blocked_processes[pid] = false;
}

void unblockSemProcess(sem_type *sem){
    char counter = 0;
    while(sem_blocked_processes[i] != sem && counter < MAX_PROCESSES){
        i = (i+1)%MAX_PROCESSES;
        counter++;
    }
    if(sem_blocked_processes[i] == sem){
        unblockProcess(i);
        sem_blocked_processes[i] = NULL;
    }
    i = (i+1)%MAX_PROCESSES;

}

void setProcessState(size_t pid, int state) {
    if (pid < 0 || pid >= MAX_PROCESSES) return;
    active_processes[pid].state = state;

}

int getProcessState(size_t pid) {
    return active_processes[pid].state;
}

size_t getFreePID() {
    for (int i=1;i<MAX_PROCESSES;i++) // halt pid = 0
        if (active_processes[i].alive == false) 
            return i;
}

char* getName(size_t pid) {
    return active_processes[pid].name;
}

unsigned long getRBP(size_t pid) {
    return active_processes[pid].rbp;
}

int startProcess(int priority, void (* process), char argc, char* argv[], char foreground, char* name) {
    _cli();
    void * processMemory = allocMemory(PAGE_SIZE);
    unsigned long rsp = prepare_process(processMemory + PAGE_SIZE, process, argc, argv);
    processType newProcess = {name, true, rsp, ready, argc, argv, processMemory};
    size_t pid = getFreePID();
    active_processes[pid] = newProcess;
    addProcess(priority, rsp, pid, foreground);
    _sti();
    return pid;
}


void enableShell() {
    setProcessState(SHELL_PID,ready); 
    setForegroundPID(SHELL_PID); 
}


int endProcess(size_t pid) {
    _cli();
    if (pid == HALT_PID || pid == SHELL_PID || pid > MAX_PROCESSES) return -1;
    if (active_processes[pid].alive == false) return -2;
    active_processes[pid].alive = false;
    setProcessState(pid, exited);
    removeProcess(pid);
    freeMemory(active_processes[pid].dataMemory);
    enableShell();
    _stint20();
    return 0;
}

void exitProcess() {
    endProcess(getRunningPID());
}

int killProcess(size_t pid) {
    return endProcess(pid);
}

void halting() {
    while(1) {
        _hlt();
    }
}

unsigned long prepareHalt() { // pid = 0 is halt
    void * haltMemory = allocMemory(HALT_PAGE);
    unsigned long haltrsp = prepare_process(haltMemory + HALT_PAGE, &halting, 1, (char**){"halt"});
    processType haltProcess = {"halt", true, haltrsp, ready, 1, (char*){"halt"}, haltMemory};
    active_processes[HALT_PID] = haltProcess;
    return haltrsp;
}


