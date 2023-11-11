#define _PROCESS_H_

#define MAX_PROCESSES 10

void initProcesses();
void printProcesses();
int launchProcess(int priority, char* name, char argc, char* argv[], char foreground);
void printCurrentPID();

/* PROCESSES */

void testingProcess(char argc, char* argv[]);
void infiniteProcess(char argc, char* argv[]);
void testPhil(uint64_t argc, char *argv[]);