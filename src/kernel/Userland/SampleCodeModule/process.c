#include <syscalls.h>
#include <library.h>

void startProcess(int priority, void (* process)(char, char*[]), char argc, char* argv[], char foreground) {
    syscall_startProcess(priority, process, argc, argv, foreground);
}

void highPriorityProcess(char argc, char* argv[]) {
    printf("High priority process started:\n argc: %d PID: %d\n",argc,syscall_getpid());
    
    for(int i=0;i<argc;i++) {
        printf("Argv: %d: %s\n",i,argv[i]);
    }
    syscall_exit();
}

void printCurrentPID() {
    printf("Current PID: %d\n",syscall_getpid());
}

void testHighPriorityProcess(char argc, char* argv[]) {
    startProcess(0, &highPriorityProcess, argc, argv, 0);
}

void testForegroundHighPriorityProcess() {
    char * argv[] = {"argument1","argument2","argument3","argument4","argument5"};
    startProcess(0, &highPriorityProcess, 5,argv, 1);
}