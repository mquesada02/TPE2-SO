#include <syscalls.h>
#include <process.h>
#include <library.h>
#include <test_util.h>
#include <stdint.h>

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

int pipeProcess(int priority, void (* process)(char, char*[]), char argc, char* argv[], char foreground, char* name, char stdin, char stdout) {
  struct processStartSTD * ps = allocMemory(sizeof(struct processStartSTD));
  ps->foreground = foreground;
  ps->name = name;
  ps->stdin = stdin;
  ps->stdout = stdout;
  int value = syscall_pipeProcess(priority, process, argc, argv, ps);
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
    loadProcess("sleep","Sleeping process", &sleepingProcess);
    loadProcess("wait","Waiting process", &waitingProcess);
    loadProcess("looping","Infinite loop", &infiniteProcess);
    loadProcess("testMem","Memory testing process", &test_mm);
    loadProcess("waitpid","Testing waitpid", &testWaitPID);
    loadProcess("testProcesses","Test the processes functions", &test_processes);
    loadProcess("testPhil","Test the philosophers problem", &testPhil);
    loadProcess("testPipes","Test the pipes", &testPipes);
    loadProcess("pipeSender","Pipe sender for testing the pipes", &pipeSender);
    loadProcess("cat","Prints the content of standard input", &cat);
    loadProcess("wc","Prints the number of lines of standard input", &wordcount);
    loadProcess("filter","Filter the vocals from standard input", &filter);
    loadProcess("loop", "Periodically prints the PID asking for help", &loop);
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

int launchPipeProcess(int priority, char* name, char argc, char* argv[], char foreground, char stdin, char stdout) {
    for(int i=0;i<processesCount;i++) {
        if (strcmp(processes[i].name,name)) {
            return pipeProcess(priority, processes[i].process, argc, argv, foreground, processes[i].name, stdin, stdout);
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

void loop(char argc, char* argv[]) {
    size_t pid = syscall_getpid();
    while(1) {printf("Hey! I'm currently stuck in this infinte loop. My PID is %d if you want to help me. Please.\n",pid); syscall_wait(5);};
}

void infiniteProcess(char argc, char* argv[]) {
    size_t pid = syscall_getpid();
    while(1) {};
}

// ---------------------------------------------- TESTS ----------------------------------------------

extern void * memset(void * destiny, int32_t c, uint64_t length);

#define MAX_BLOCKS 128
#define MAX_CYCLES 10

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

  for(int i=0;i<MAX_CYCLES;i++) {
    rq = 0;
    total = 0;
    // Request as many blocks as we can
    while (rq < MAX_BLOCKS && total < max_memory) {
      mm_rqs[rq].size = GetUniform(max_memory - total - 1) + 1;
      mm_rqs[rq].address = allocMemory(mm_rqs[rq].size);
      printf("Alloc'd %x\n", mm_rqs[rq].address);
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
        printf("Free'd %x\n", mm_rqs[i].address);
      } 
  //}
  }
  syscall_exit();
}

// --------------------------------------------------------------------------------------------//

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
      p_rqs[rq].pid = launchProcess(1,"looping",0,argvAux,0);
      if (p_rqs[rq].pid == 0 || p_rqs[rq].pid == 1) {
        printf("test_processes: ERROR creating process\n");
        syscall_exit();
      } else {
        p_rqs[rq].state = RUNNING;
        alive++;
      }
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

void testWaitPID(char argc, char* argv[]) {
  /* recibe la cantidad de hijos que se desean esperar */
  if (argc != 1)
    syscall_exit();
  int children = strToNum(argv[0]);
  size_t children_pids[children];
  char* timeArgv[] = {"5"};
  for (int i=0;i<children;i++) {
    children_pids[i] = launchProcess(1,"sleep",1,timeArgv,0);
  }
  printf("Waiting process started:\nPID: %d\n",syscall_getpid());
  syscall_waitpid(-1);
  printf("Waiting process ended:\nPID: %d\n",syscall_getpid());
  syscall_exit();
}

void sleepingProcess(char argc, char* argv[]) {
  if (argc != 1)
    syscall_exit();
  printf("Sleeping process started:\nPID: %d\n",syscall_getpid());
  syscall_wait(strToNum(argv[0])+1);
  printf("Sleeping process ended:\nPID: %d\n",syscall_getpid());
  syscall_exit();
}

void waitingProcess(char argc, char* argv[]) {
  if (argc != 1)
    syscall_exit();
  printf("Waiting process started:\nPID: %d\n",syscall_getpid());
  syscall_waitpid(strToNum(argv[0]));
  printf("Waiting process ended:\nPID: %d\n",syscall_getpid());
  syscall_exit();
}

// --------------------------------------------------------------------------------------------//
void pipeSender(char argc, char * argv[]) {
  while(1) {
    printf("Message sent from PID %d!\n",syscall_getpid());
    sleep(5);
  }
}

void testPipes(char argc, char * argv[]) {
  char buffer[128];
  while(1) {
    getInput(buffer, argv[argc-1][0]=='*');
    printf("\nPID %d Input: %s\n",syscall_getpid(), buffer);
  }
}

void cat(char argc, char* argv[]) {
    char buffer[128];
    while(1) {
        catInput(buffer, argv[argc-1][0]=='*');
        printf("%s\n",buffer);
    }
}

void wordcount(char argc, char* argv[]) {
  printf("Number of lines: %d", getWC(0));
  syscall_exit();
}

void filter(char argc, char* argv[]) {
  char buffer[128];
  while(1) {
    filterInput(buffer, argv[argc-1][0]=='*');
    printf("%s\n",buffer);
  }
}