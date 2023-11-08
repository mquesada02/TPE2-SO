#define _PROCESS_H_

#define MAX_PROCESSES 10

void initProcesses();
void printProcesses();
void launchProcess(int priority, char* name, char argc, char* argv[], char foreground);
void printCurrentPID();

/* PROCESSES */

void testingProcess(char argc, char* argv[]);
void infiniteProcess(char argc, char* argv[]);