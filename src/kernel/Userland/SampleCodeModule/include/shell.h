#ifndef _SHELL_H_
#define _SHELL_H_

#define TOTAL_MODULES 16
#define REGISTERS_COUNT 16
#define DIV_BUFF_SIZE 12
#define COMMAND_MIN_SIZE 4
#define TIME_BUFF_SIZE 9

void startShell();
void loadModule(char * name, char * description, void (*function)(void));
void loadAllModules();
void runModule(const char * input, char argc, char * argv[]);
void printHelp(char argc, char * argv[]);
void printTime();
void printRegisters();
void divide();
void clear();
void testRegisters();
void mem();
void pstart(char argc, char * argv[]);
void printPID();
void printCurrentProcesses();
void killProcess(char argc, char * argv[]);
void blockProcess(char argc, char * argv[]);
void changePriority(char argc, char * argv[]);
void looping(char argc, char* argv[]);
void phylo(char argc, char* argv[]);
#endif