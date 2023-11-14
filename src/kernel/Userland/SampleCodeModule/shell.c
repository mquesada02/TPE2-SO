// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <shell.h>
#include <library.h>
#include <pong.h>
#include <stdint.h>
#include <MemoryManagerADT.h>
#include <syscalls.h>
#include <process.h>
#include <test_util.h>


#define NULL (void*) 0

extern void invalidOperation();
extern void loopRegisters();

/**
 * @brief Estructura que contiene el nombre, la descripción y la dirección de la función correspondinte al módulo.
 */
typedef struct {
    char * name;
    char * description;
    void (*function)(char, char *[]);     
} module;

/**
 * @brief Vector de módulos
 */
module modules[TOTAL_MODULES];
int modulesCount = 0;

/**
 * @brief Función que inicializa la Shell, y consulta constantemente acerca de qué módulo se desea correr.
 */
void startShell() {
    loadAllModules();
    initProcesses();
    printf("Welcome to the shell\n");
    modules[0].function(0,NULL);
    char input[COMMAND_MAX_SIZE];
    char **params = syscall_allocMemory(MAX_PARAMETERS*sizeof(char*));
    for(int i=0;i<MAX_PARAMETERS;i++) {
        params[i] = syscall_allocMemory(PARAMETER_MAX_SIZE*sizeof(char));
    }
    char command[COMMAND_MAX_SIZE];
    char argc;
    char pipeIndex;
    while(1){
        printf("\n");
        print("$ ", BLUE);
        getInput(input, 0);
        argc = parseInput(input, command, params);
        pipeIndex = getPipeIndex(argc, params);
        runModulePipe(command,argc,params,pipeIndex);
    }
}

/**
 * @brief Para cada módulo, almacena su nombre, una breve descripción de lo que hace, y un puntero a su función
 * correspondiente.
 * 
 * @param name Nombre del módulo a almacenar.
 * @param description Breve descripción del módulo.
 * @param function Puntero a la función correspondiente.
 */
void loadModule(char * name, char * description, void (*function)(char, char*[])) {
    modules[modulesCount].name = name;
    modules[modulesCount].description = description;
    modules[modulesCount].function = function;
    modulesCount++;
}

/**
 * @brief Carga todas los módulos/funcionalidades de la Shell disponibles para el usuario.
 */
void loadAllModules() {
    loadModule("help", "Prints name and description for all the functionalities available for the user", &printHelp);
    loadModule("time", "Prints the current time of the system", &printTime);
    //loadModule("registers", "Prints the values of the processor's registers at the instant they were saved. Registers are saved by pressing Alt", &printRegisters);
    loadModule("divide", "Asks for two numbers and prints the result of dividing one by the other", &divide);
    //loadModule("pong", "Starts the arcade game Pong for two players", &playPong);
    loadModule("clear", "Clears the screen of the shell", &clear);
    //loadModule("invop", "Performs an invalid assembly operation (mov cr6, 0x77) and throws an invalid operation exception", &invalidOperation);
    //loadModule("testReg", "Sets all registers(except r12, rsp and rbp) at 33h and gives time for pressing 'Alt' and testing the functionality 'registers'", &testRegisters);
    loadModule("mem", "Prints the memory status", &mem);
    loadModule("pid", "Prints the pid of the current process", &printPID);
    loadModule("pstart", "Start a new process given a priority and process name", &pstart);
    loadModule("kill","Kill a process given a PID", &killProcess);
    loadModule("ps","Prints all processes", &printCurrentProcesses);
    loadModule("block","Switch the process status between blocked and ready given a PID", &blockProcess);
    loadModule("nice", "Change the priority of a process given a PID and a priority", &changePriority);
    loadModule("loop","Periodically prints the PID asking for help", &loopProcess);
    loadModule("cat", "Alias for pstart 1 cat", &catProcess);
    loadModule("wc", "Alias for pstart 1 wc", &wcProcess);
    loadModule("filter", "Alias for pstart 1 filter", &filterProcess);
    loadModule("phylo","Starts the philosophers problem and periodically prints the state of the table", &phylo);
}   

/**
 * @brief Función que llama al módulo correspondiente dependiendo del parámetro ingresado. En caso de
 * no existir, imprime un mensaje de error.
 * 
 * @param input Nombre del módulo que se busca comparar.
 */
void runModule(const char * input, char argc, char * params[]){
    printf("\n");    
    for(int i=0;i<TOTAL_MODULES;i++){
        if (strcmp(modules[i].name,input)){
            modules[i].function(argc,params);
            return;
        }
    }
    printf("Invalid functionality: for a list of available functionalities type 'help'");
    return;
}

void runProcessAliasPipe(char * input, char argc, char * params[]) {
    /* input: process name for pstart */
    char ** argv = syscall_allocMemory(MAX_PARAMETERS*sizeof(char*));
    argv[0] = "1";
    argv[1] = input;
    int pipeIndex = getPipeIndex(argc, params);
    if (pipeIndex != -1) {

        int fds[2];
        int pipe = syscall_pipe(fds);
        if (pipe == -1) {
            printf("Error creating pipe.\n");
            return;
        }

        for(int i=2;i<pipeIndex+2;i++) {
            argv[i] = params[i-2];
        }        
        pstartPipe(pipeIndex+2, argv, 0, fds[0]);
        argv+=(pipeIndex+2)+1;
        params+=pipeIndex+1;
        argv[0] = "1";
        for (int i=1; i<argc-(pipeIndex+1)+1; i++) {
            argv[i] = params[i-1];
        }
        argv[argc-(pipeIndex+1)+1] = "*";
        argv[argc-(pipeIndex+1)+2] = "&";
        pstartPipe(argc-(pipeIndex+1)+3, argv, fds[0], 1);
        return;
    } else {
        for(int i=2;i<argc+2;i++) {
            argv[i] = params[i-2];
        }
        pstart(argc+2, argv);
        return;
    }

}

void catProcess(char argc, char * argv[]) {
    runProcessAliasPipe("cat", argc, argv);
    return;
}

void wcProcess(char argc, char * argv[]) {
    runProcessAliasPipe("wc", argc, argv);
    return;
}

void filterProcess(char argc, char * argv[]) {
    runProcessAliasPipe("filter", argc, argv);
    return;
}

void loopProcess(char argc, char * argv[]) {
    runProcessAliasPipe("loop", argc, argv);
    return;
}

void runModulePipe(const char * input, char argc, char * params[], int pipeIndex) {
    if (pipeIndex == -1 || strcmp(input,"pstart") == 0) {
        runModule(input,argc,params);
        return;
    }
    /* el comando es pstart */
    int fds[2];
    int pipe = syscall_pipe(fds);
    if (pipe == -1) {
        printf("Error creating pipe.\n");
        return;
    }
    (params+pipeIndex+1)[argc-(pipeIndex+1)] = "*"; // al segundo proceso lo mando al bg siempre
    (params+pipeIndex+1)[argc-(pipeIndex+1)+1] = "&";
    pstartPipe(argc-(pipeIndex+1)+1+1, params+(pipeIndex+1), fds[0], 1);
    pstartPipe(pipeIndex, params, 0, fds[0]);
    

    /* int pid1 = pstart(pipeIndex, params);
    syscall_open(pid1, fds[1]);
    syscall_dupstdout(pid1, fds[1]);
    int pid2 = pstart(argc-(pipeIndex+1), params+(pipeIndex+1));
    syscall_open(pid2, fds[0]);
    syscall_dupstdin(pid2, fds[0]);
     */
    return;
}

// -------------------------------------- FUNCIONES DE LOS MÓDULOS -------------------------------------- //

/**
 * @brief Función correspondiente para el módulo de "help". Imprime todas las funcionalidades disponibles.
 */
void printHelp(char argc, char * argv[]) {
    printf("The shell's functionalities are the following:\n");
    if (argc == 0 || (argc == 1 && argv[0][0] == '1')) {
        for(int i=0; i<TOTAL_MODULES/2; i++) {
            printf("\n");
            print(" - ", GREEN);
            print(modules[i].name, GREEN);
            printf(" : ");
            printf("%s",modules[i].description);
        }
    } else if (argc == 1 && argv[0][0] == '2') {
        for(int i=TOTAL_MODULES/2; i<TOTAL_MODULES; i++) {
            printf("\n");
            print(" - ", GREEN);
            print(modules[i].name, GREEN);
            printf(" : ");
            printf("%s",modules[i].description);
        }
    } else {
        printf("Invalid page number. Valid pages are 1 and 2.");
    }
    
    printf("\n");
    printf("\nEnter the chosen functionality's name through the command line and press enter to run it.\nTo see more commands use 'help <page>'");
}

/**
 * @brief Función correspondiente para el módulo de "time". Imprime el valor de la hora actual del sistema.
 */
void printTime(char argc, char * argv[]) {
    char timeStr[TIME_BUFF_SIZE];
    printf("The current time is ");
    getTime(timeStr);
    printf("%s",timeStr);
}

/**
 * @brief Función correspondiente para el módulo de "registers". Imprime el valor de los registros cuando se
 * guardaron al presionar la tecla Alt. Si no se presionó en ningún momento, imprime un mensaje correspondiente.
 */
void printRegisters(char argc, char * argv[]) {
    long int * registers = getRegisters();
    if (registers == 0){
        printf("No registers saved. Press 'Alt' to save registers.");
        return;
    }
    printf("These are the registers' hexadecimal values:");
    char * register_names[REGISTERS_COUNT] = {"RAX", "RBX", "RCX", "RDX", "RDI", "RSI", 
    "RBP", "RSP", "R8 ", "R9 ", "R10", "R11", "R12", "R13", "R14", "R15"};
    char buffer[10];
    for (int i=0; i<REGISTERS_COUNT; i++) {
        printf("\n");
        print(register_names[i], ORANGE);
        printf(" = ");
        numToStr(registers[i], 16, buffer);
        printf("%s",buffer);
        printf("h");
    }
}

/**
 * @brief Función correspondiente para el módulo de "divide". Consulta al usuario dos veces para que ingrese
 * dos números: un dividendo y un divisor. Luego, se imprime en pantalla el valor del cociente y el resto al
 * hacer esa división.
 */
void divide(char argc, char* argv[]) {
    char numberStr[DIV_BUFF_SIZE];
    char divisorStr[DIV_BUFF_SIZE];
    char quotientStr[DIV_BUFF_SIZE];
    char remainderStr[DIV_BUFF_SIZE];
    printf("Insert a number: ");
    getInput(numberStr, 0);
    printf("\n");
    printf("Insert a divisor: ");
    getInput(divisorStr, 0);
    int number = strToNum(numberStr);
    int divisor = strToNum(divisorStr);
    int quotient = number/divisor; 
    int remainder = number%divisor;
    numToStr(quotient, 10, quotientStr);
    numToStr(remainder, 10, remainderStr);
    printf("\n");
    printf("Quotient: ");
    printf("%s",quotientStr);
    printf("\n");
    printf("Remainder: ");
    printf("%s",remainderStr);
}

/**
 * @brief Función correspondiente al módulo de "clear". Limpia la pantalla de la shell.
 */
void clear(char argc, char* argv[]) {
    enableDoubleBuffer(1);
    enableDoubleBuffer(0);
}

/**
 * @brief Función correspondiente al módulo de "test registers". Imprime un mensaje en pantalla 
 * indicando al usuario que presione alt, y llama a la función loopRegisters, en la cual se setean
 * los registros.
 * 
 */
void testRegisters(char argc, char * argv[]){
    printf("Press 'Alt' now! (and wait a few seconds)");
    loopRegisters();
}

void mem(char argc, char * argv[]) {
    printMemStatus();
}

int pstartPipe(char argc, char * argv[], char stdin, char stdout) {
    if (strcmp(argv[0],"-a")) {
        printProcesses();
        return MAX_PROCESSES;
    }
    if (argc < 2) {
        printf("Usage: pstart <priority> <process> [args]\n To get available processes, type 'pstart -a'\n");
        return MAX_PROCESSES;
    }
    int priority = strToNum(argv[0]);
    if (priority < 1 || priority > 4) {
        printf("Invalid priority. Valid priorities are 1, 2, 3 and 4.\n");
        return MAX_PROCESSES;
    }
    char foreground = argv[argc-1][0]=='&';
    return launchPipeProcess(priority, argv[1], argc-(2+foreground), argv+2, !foreground, stdin, stdout);
}

void pstart(char argc, char * argv[]) {
    if (strcmp(argv[0],"-a")) {
        printProcesses();
        return;
    }
    if (argc < 2) {
        printf("Usage: pstart <priority> <process> [args]\n To get available processes, type 'pstart -a'\n");
        return;
    }
    int priority = strToNum(argv[0]);
    if (priority < 1 || priority > 4) {
        printf("Invalid priority. Valid priorities are 1, 2, 3 and 4.\n");
        return;
    }
    char foreground = argv[argc-1][0]=='&';
    launchProcess(priority, argv[1], argc-(2+foreground), argv+2, !foreground);
}


void printPID(char argc, char* argv[]) {
    printCurrentPID();
}

void killProcess(char argc, char * argv[]) {
    int value = syscall_kill(strToNum(argv[0]));
    if (value == -1) {
        printf("Invalid PID.\n");
    } else if (value == -2) {
        printf("Invalid PID. The process is not alive.\n");
    } else {
        printf("Killed the process with PID %s.\n",argv[0]);
    }
}

void printCurrentProcesses(char argc, char * argv[]) {
    syscall_ps();
}

void blockProcess(char argc, char * argv[]) {
    int value = syscall_switchBlock(strToNum(argv[0]));
    if (value == -1) {
        printf("Invalid PID.\n");
    } else if (value == -2) {
        printf("Invalid PID. The process is not alive.\n");
    } else {
        printf("Switched the process status with PID %s.\n",argv[0]);
    }
}

void changePriority(char argc, char * argv[]) {
    int value = syscall_changePriority(strToNum(argv[0]), strToNum(argv[1]));
    if (value == -1) {
        printf("Invalid PID.\n");
    } else if (value == -2) {
        printf("Invalid PID. The process is not alive.\n");
    } else if (value == -3) {
        printf("Invalid priority. Valid priorities are 1, 2, 3 and 4.\n");
    } else {
        printf("Changed the priority of the process with PID %s to %s.\n",argv[0],argv[1]);
    }
}


//-------philosophers----------------------------------------------
#define N 25 /* número mäximo de filósofos */

void testPhil(char argc, char *argv[]) {
    if(argc != 1){
      printf("Invalid number of arguments: must provide initial number of philosophers\n");
      syscall_exit();
    }

    int cant = satoi(argv[0]);

    if(cant<2 || cant>N){
      printf("Invalid number of philosophers: must be between 2 and %d\n", N);
      syscall_exit();
    }

    syscall_createPhils(cant); 

    char c;
    while(1){
        c = getChar();
        if(c == 'a'){
            syscall_createPhil();
        }
        if(c=='r'){
            syscall_removePhil();
        }
    }
    // while(1){
    //   write_state();
    //   sleep(3);
    // }
}

void phylo(char argc, char* argv[]) {
    char foreground = argv[argc-1][0]=='&';
    startProcess(1, &testPhil, argc-(foreground), argv, !foreground, "testPhil");
}