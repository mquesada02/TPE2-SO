#include <processes.h>
#include <scheduler.h>
#include <interrupts.h>
#include <MemoryManager.h>
#include <philosophers.h>

#include <lib.h>

#define PAGE_SIZE 4096
#define HALT_PAGE 2048

#define false 0
#define true 1

enum state {running, ready, blocked, exited};

typedef struct process {
    char* name;
    char alive;
    unsigned long rbp;
    int state;
    char argc;
    char** argv;
    void * dataMemory;
    size_t parentPID;
    char childrenCount;
    char children[MAX_PROCESSES];
    char stdin;
    char stdout;
    char openFDs[MAX_FDS];
} processType;

processType active_processes[MAX_PROCESSES+1];
char keyboard_blocked_processes[MAX_PROCESSES];
size_t waitchild_blocked_processes[MAX_PROCESSES];
void * sem_blocked_processes[MAX_PROCESSES];
char waiting_sem_locked[MAX_PROCESSES];
int fd_blocked_processes[MAX_PROCESSES];
static char j = 0;

int testPhilPID = -1;
int getTestPhilPID() {
    return testPhilPID;
}
void setTestPhilPID(int pid) {
    testPhilPID = pid;
}

extern unsigned long prepare_process(void * memory, void (* process), char argc, char* argv[]);

void initProcesses() {
    for (int i=0;i<MAX_PROCESSES;i++) {
        active_processes[i].alive = false;
        active_processes[i].rbp = 0;
        active_processes[i].state = ready;
        active_processes[i].argc = 0;
        active_processes[i].argv = NULL;
        active_processes[i].dataMemory = NULL;
        active_processes[i].parentPID = 0;
        active_processes[i].stdin = 0;
        active_processes[i].stdout = 1;
        for (int j=0;j<MAX_PROCESSES;j++) {
            active_processes[i].children[j] = 0;
        }
        for (int j=2;j<MAX_FDS;j++) {
            active_processes[i].openFDs[j] = 0;
        }
        active_processes[i].childrenCount = 0;
        keyboard_blocked_processes[i] = 0;
        waitchild_blocked_processes[i] = 0;
        sem_blocked_processes[i] = NULL;
        waiting_sem_locked[i] = 0;
        fd_blocked_processes[i] = -1;
    }
    active_processes[MAX_PROCESSES].alive = false;
    active_processes[MAX_PROCESSES].state = exited;
    active_processes[MAX_PROCESSES].stdin = 0;
    active_processes[MAX_PROCESSES].stdout = 1;
    active_processes[MAX_PROCESSES].openFDs[0] = 1;
    active_processes[MAX_PROCESSES].openFDs[1] = 1;
}

int isOpenedFD(size_t pid, int fd) {
    return active_processes[pid].openFDs[fd];
}

char getSTDIN(size_t pid) {
    return active_processes[pid].stdin;
}

char getSTDOUT(size_t pid) {
    return active_processes[pid].stdout;
}

void setSTDIN(size_t pid, char stdin) {
    if (!isOpenedFD(pid, stdin)) return;
    active_processes[pid].stdin = stdin;
}

void setSTDOUT(size_t pid, char stdout) {
    if (!isOpenedFD(pid, stdout)) return;
    active_processes[pid].stdout = stdout;
}

void openFD(size_t pid, int fd) {
    active_processes[pid].openFDs[fd] = 1;
}

void closeFD(size_t pid, int fd) {
    active_processes[pid].openFDs[fd] = 0;
}



int switchBlock(size_t pid) {
    if(pid <= SHELL_PID || pid >= MAX_PROCESSES) return -1;
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


void addChildren(size_t parent, size_t child) {
    active_processes[parent].children[active_processes[parent].childrenCount] = child;
    active_processes[parent].childrenCount++;
}

void removeChildren(size_t parent, size_t child) {
    int index = -1;
    for(int i=0;i<active_processes[parent].childrenCount;i++) {
        if (active_processes[parent].children[i] == child) {
            index = i;
            break;
        }
    }
    if (index == -1) return;
    for(int i=index;i<active_processes[parent].childrenCount-1;i++) {
        active_processes[parent].children[i] = active_processes[parent].children[i+1];
    }
}

void waitPID(int pid) {
    if (pid == HALT_PID || pid >= MAX_PROCESSES) return;
    //if (active_processes[pid].alive == false) return;
    size_t rpid = getRunningPID();
    if (pid == -1) {
        /* wait for children to end */
        for(int i=0;i<active_processes[rpid].childrenCount;i++) {
            if (active_processes[active_processes[rpid].children[i]].alive) {
                waitchild_blocked_processes[rpid] = active_processes[rpid].children[i];
                blockProcess(rpid);
            }
        }
    } else {
        /* wait for specific pid to end */
        waitchild_blocked_processes[rpid] = pid;
        blockProcess(rpid);
    }
    return;
}

int isAlive(size_t pid) {
    return active_processes[pid].alive;
}

int isBlocked(size_t pid) {
    return getProcessState(pid) == blocked;
}

int isBlockedFD(size_t pid, int fd) {
    return fd_blocked_processes[pid] == fd;
}

void blockFD(size_t pid, int fd) {
    fd_blocked_processes[pid] = fd;
    blockProcess(pid);
}

void unblockFD(size_t pid, int fd) {
    if (!isBlockedFD(pid, fd)) return;
    fd_blocked_processes[pid] = -1;
    unblockProcess(pid);
}

void blockProcess(size_t pid) {
    setProcessState(pid, blocked);
    _stint20();
}

void unblockProcess(size_t pid) {
    setProcessState(pid, ready);
}

void blockKeyboardProcess(size_t pid) {
    if (pid < 0 || pid >= MAX_PROCESSES) return;
    blockProcess(pid);
    keyboard_blocked_processes[pid] = true;
}

void blockSemProcess(size_t pid, sem_type * sem, char waiting_locked) {
    if (pid < 0 || pid >= MAX_PROCESSES) return;
    blockProcess(pid);
    sem_blocked_processes[pid] = sem;
    waiting_sem_locked[pid] = waiting_locked;
}

int isKBlocked(size_t pid) {
    return keyboard_blocked_processes[pid];
}

void unblockKeyboardProcess(size_t pid) {
    if (pid < 0 || pid >= MAX_PROCESSES) return;
    unblockProcess(pid);
    keyboard_blocked_processes[pid] = false;
}

int unblockSemProcess(sem_type *sem, char waiting_locked){
    char counter = 0;
    while(counter < MAX_PROCESSES){
        if(sem_blocked_processes[j] == sem && waiting_sem_locked[j] == waiting_locked)
            break;
        j = (j+1)%MAX_PROCESSES;
        counter++;
    }
    if(sem_blocked_processes[j] == sem){
        unblockProcess(j);
        sem_blocked_processes[j] = NULL;
        waiting_sem_locked[j] = 0;
        j = (j+1)%MAX_PROCESSES;
        return true;
    }
    //si no habia ningun proceso bloqueado con ese semaforo
    return false;
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
    processType newProcess = {name, true, rsp, ready, argc, argv, processMemory, getRunningPID(), 0, {0}, getSTDIN(getRunningPID()), getSTDOUT(getRunningPID()), {1,1}};
    size_t pid = getFreePID();
    active_processes[pid] = newProcess;
    addProcess(priority, rsp, pid, foreground);
    addChildren(getRunningPID(), pid);
    if(name == "testPhil"){
        setTestPhilPID(pid);
    }
    _sti();
    return pid;
}

int pipeProcess(int priority, void (* process), char argc, char* argv[], char foreground, char* name, char stdin, char stdout) {
    _cli();
    void * processMemory = allocMemory(PAGE_SIZE);
    unsigned long rsp = prepare_process(processMemory + PAGE_SIZE, process, argc, argv);
    processType newProcess = {name, true, rsp, ready, argc, argv, processMemory, getRunningPID(), 0, {0}, stdin, stdout, {0}};
    newProcess.openFDs[stdin] = 1;
    newProcess.openFDs[stdout] = 1;
    size_t pid = getFreePID();
    active_processes[pid] = newProcess;
    addProcess(priority, rsp, pid, foreground);
    addChildren(getRunningPID(), pid);
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
    removeChildren(active_processes[pid].parentPID, pid);
    for(int i=0;i<MAX_PROCESSES;i++) {
        if (waitchild_blocked_processes[i] == pid) {
            unblockProcess(i);
            waitchild_blocked_processes[i] = 0;
        }
    }
    freeMemory(active_processes[pid].dataMemory);
    if(pid == getTestPhilPID()){
        setTestPhilPID(-1);
        kill_philosophers();
    }
    if(active_processes[pid].name != "philosopher"){
        enableShell();
    }
    _stint20();
    return 0;
}

void exitProcess() {
    write(getSTDOUT(getRunningPID()), -1, 0xFFFFFF, 0x000000);
    /* send eof to stdout too*/
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
    processType haltProcess = {"halt", true, haltrsp, ready, 1, (char*){"halt"}, haltMemory, getRunningPID(), {0}, 0, 1};
    active_processes[HALT_PID] = haltProcess;
    return haltrsp;
}

//---------------------------philosophers--------------------------------

void philosopher(uint64_t argc, char *argv[]) /* 1 parametro: número de filósofo*/
{
  if (argc != 1)
    exitProcess();

  int i = satoi(argv[0]);

  while(1){ /* se repite en forma indefinida */
    think(); /* el filósofo está pensando */
    take_forks(i); /* adquiere dos tenedores o se bloquea */
    eat(); /* come espagueti */
    leave_forks(i); /* pone de vuelta ambos tenedores en la mesa */
  }
}

void createPhils(int cant){
    mutex = sem_open("mutex_phil", 1);
    for(int i =0; i<cant; i++){
        createPhil();
    }
}

void createPhil(){
  if(last_i >= N)
    return;
  char ** buffer = allocMemory(sizeof(char*));
  buffer[0] = allocMemory(3*sizeof(char));
  numToStr(last_i, 10, buffer[0]);

  char aux[7] = {'p', 'h', 'i', 'l'};
  ph_state[last_i] = THINKING;
  aux[4] = buffer[0][0];
  aux[5] = buffer[0][1];
  aux[6] = '\0';
  s[last_i] = sem_open(aux, 1);

  philosopherPIDs[last_i] = startProcess(2, &philosopher, 1, buffer, 0, "philosopher");
  last_i++;
  freeMemory(buffer[0]);
  freeMemory(buffer);
}

void removePhil(){
  if(last_i <= 2)
    return;
  last_i--;
  killProcess(philosopherPIDs[last_i]);
  sem_close(s[last_i]);
}

void kill_philosophers(){
    for (int i = 0; i < last_i; i++){
        sem_close(s[i]);
        killProcess(philosopherPIDs[i]);
    }
    last_i = 0;
    sem_close(mutex);
}
