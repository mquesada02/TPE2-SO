#ifndef _SHELL_H_
#define _SHELL_H_

#define TOTAL_MODULES 16
#define REGISTERS_COUNT 16
#define DIV_BUFF_SIZE 12
#define COMMAND_MIN_SIZE 4
#define TIME_BUFF_SIZE 9

void startShell();
void loadModule(char * name, char * description, void (*function)(char, char *[]));
void loadAllModules();
void runModule(const char * input, char argc, char * argv[]);
void runModulePipe(const char * input, char argc, char * params[], int pipeIndex);
void printHelp(char argc, char * argv[]);
void printTime(char argc, char * argv[]);
void printRegisters(char argc, char * argv[]);
void divide(char argc, char * argv[]);
void clear(char argc, char * argv[]);
void testRegisters(char argc, char * argv[]);
void mem(char argc, char * argv[]);
void pstart(char argc, char * argv[]);
int pstartPipe(char argc, char * argv[], char stdin, char stdout);
void printPID(char argc, char* argv[]);
void printCurrentProcesses(char argc, char* argv[]);
void killProcess(char argc, char * argv[]);
void blockProcess(char argc, char * argv[]);
void changePriority(char argc, char * argv[]);
void catProcess(char argc, char * argv[]);
void wcProcess(char argc, char * argv[]);
void filterProcess(char argc, char * argv[]);
void loopProcess(char argc, char * argv[]);
void phylo(char argc, char* argv[]);
#endif