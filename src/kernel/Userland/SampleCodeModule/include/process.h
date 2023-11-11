#define _PROCESS_H_

#define MAX_PROCESSES 10

void initProcesses();
void printProcesses();
int startProcess(int priority, void (* process)(char, char*[]), char argc, char* argv[], char foreground, char* name);
int launchProcess(int priority, char* name, char argc, char* argv[], char foreground);
void printCurrentPID();

/* PROCESSES */

void testingProcess(char argc, char* argv[]);
void infiniteProcess(char argc, char* argv[]);
void sleepingProcess(char argc, char* argv[]);
void waitingProcess(char argc, char* argv[]);
void testWaitPID(char argc, char* argv[]);