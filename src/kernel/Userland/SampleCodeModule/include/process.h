#define _PROCESS_H_

#define MAX_PROCESSES 40

void initProcesses();
void printProcesses();
int startProcess(int priority, void (* process)(char, char*[]), char argc, char* argv[], char foreground, char* name);
int launchProcess(int priority, char* name, char argc, char* argv[], char foreground);
int launchPipeProcess(int priority, char* name, char argc, char* argv[], char foreground, char stdin, char stdout);
void printCurrentPID();

/* PROCESSES */

void testingProcess(char argc, char* argv[]);
void infiniteProcess(char argc, char* argv[]);
void sleepingProcess(char argc, char* argv[]);
void waitingProcess(char argc, char* argv[]);
void testWaitPID(char argc, char* argv[]);
void testPhil(uint64_t argc, char *argv[]);
void testPipes(char argc, char * argv[]);
void pipeSender(char argc, char * argv[]);
void cat(char argc, char* argv[]);
void wordcount(char argc, char* argv[]);
void filter(char argc, char* argv[]);
void loop(char argc, char* argv[]);