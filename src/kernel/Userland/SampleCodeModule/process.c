#include <syscalls.h>
#include <process.h>
#include <library.h>
#include <test_util.h>

typedef struct process {
    char* name;
    char* description;
    void (* process)(char, char*[]);
} process;

process processes[MAX_PROCESSES];
static int processesCount = 0;

int startProcess(int priority, void (* process)(char, char*[]), char argc, char* argv[], char foreground, char* name) {
    struct processStart * ps = allocMemory(sizeof(struct processStart));
    ps->foreground = foreground;
    ps->name = name;
    int value = syscall_startProcess(priority, process, argc, argv, ps);
    freeMemory(ps);
    return value;
}

void loadProcess(char* name, char* description, void (* process)(char, char*[])) {
    processes[processesCount].name = name;
    processes[processesCount].description = description;
    processes[processesCount].process = process;
    processesCount++;
    return;
}

void initProcesses() {
    loadProcess("test","Testing process", &testingProcess);
    loadProcess("loop","Infinite loop", &infiniteProcess);
    loadProcess("testMem","Memory testing process", &test_mm);
    loadProcess("testProcesses","Test the processes functions", &test_processes);
}

void printProcesses() {
    printf("All processes:\n");
    for(int i=0;i<processesCount;i++) {
        printf("- %s: %s\n",processes[i].name,processes[i].description);
    }
}

int launchProcess(int priority, char* name, char argc, char* argv[], char foreground) {
    for(int i=0;i<processesCount;i++) {
        if (strcmp(processes[i].name,name)) {
            return startProcess(priority, processes[i].process, argc, argv, foreground, processes[i].name);
        }
    }
    printf("No process with name '%s' found\n",name);
}

void printCurrentPID() {
    printf("Current PID: %d\n",syscall_getpid());
}

void testingProcess(char argc, char* argv[]) {
    printf("Testing process started:\nargc: %d\nPID: %d\n",argc,syscall_getpid());
    
    for(int i=0;i<argc;i++) {
        printf("Argument %d: %s\n",i,argv[i]);
    }
    syscall_exit();
}

void infiniteProcess(char argc, char* argv[]) {
    size_t pid = syscall_getpid();
    printf("Infinite process started:\nPID: %d\n",pid);
    while(1) {/* printf("PID:%d\n",pid); */};
}

// ---------------------------------------------- TESTS ----------------------------------------------

extern void * memset(void * destiny, int32_t c, uint64_t length);

#define MAX_BLOCKS 128

typedef struct MM_rq {
  void *address;
  uint32_t size;
} mm_rq;

uint64_t test_mm(uint64_t argc, char *argv[]) {
  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;
  uint64_t max_memory;

  if (argc != 1)
    syscall_exit();

  if ((max_memory = satoi(argv[0])) <= 0)
    syscall_exit();

  while(1) {
    rq = 0;
    total = 0;
    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory) {
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      mm_rqs[rq].address = allocMemory(mm_rqs[rq].size);
      //printf("Alloc'd %x\n", mm_rqs[rq].address);
      if (mm_rqs[rq].address) {
        total += mm_rqs[rq].size;
        rq++;
      }
    }

    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address) {
        memset(mm_rqs[i].address, i, mm_rqs[i].size);
      }

    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address)
        if (!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) {
          printf("test_mm ERROR\n");
          syscall_exit();
        }
    //printf("About to free\n");
    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address) {
        freeMemory(mm_rqs[i].address);
        //printf("Free'd %x\n", mm_rqs[i].address);
      } 
  }
}

enum State { RUNNING,
            BLOCKED,
            KILLED };

typedef struct P_rq {
int32_t pid;
enum State state;
} p_rq;

int64_t test_processes(uint64_t argc, char *argv[]) {
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;
  uint64_t max_processes;
  char *argvAux[] = {0};

  if (argc != 1)
    syscall_exit();

  if ((max_processes = satoi(argv[0])) <= 0)
    syscall_exit();

  p_rq p_rqs[max_processes];

  //while (1) {

    // Create max_processes processes
    for (rq = 0; rq < max_processes; rq++) {
      /* p_rqs[rq].pid = my_create_process("endless_loop", 0, argvAux); */
      p_rqs[rq].pid = launchProcess(1,"loop",0,argvAux,0);
      /* if (p_rqs[rq].pid == -1) {
        printf("test_processes: ERROR creating process\n");
        return -1;
      } else { */
        p_rqs[rq].state = RUNNING;
        alive++;
    }

    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0) {

      for (rq = 0; rq < max_processes; rq++) {
        action = GetUniform(100) % 2;

        switch (action) {
          case 0:
            if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED) {
              int value = syscall_kill(p_rqs[rq].pid);
              if (value == -1 || value == -2) {
                printf("test_processes: ERROR killing process. Code: %d\n",value);
                syscall_exit();
              }
              p_rqs[rq].state = KILLED;
              alive--;
            }
            break;

          case 1:
            if (p_rqs[rq].state == RUNNING) {
              int value2 = syscall_switchBlock(p_rqs[rq].pid);
              if (value2 == -1 || value2 == -2) {
                printf("test_processes: ERROR blocking process. Code: %d\n",value2);
                syscall_exit();
              }
              p_rqs[rq].state = BLOCKED;
            }
            break;
        }
      }

      // Randomly unblocks processes
      for (rq = 0; rq < max_processes; rq++)
        if (p_rqs[rq].state == BLOCKED && GetUniform(100) % 2) {
          int value3 = syscall_switchBlock(p_rqs[rq].pid);
          if (value3 == -1 || value3 == -2) {
            printf("test_processes: ERROR unblocking process. Code: %d\n",value3);
            syscall_exit();
          }
          p_rqs[rq].state = RUNNING;
        }
    }
  //}
  syscall_exit();
}