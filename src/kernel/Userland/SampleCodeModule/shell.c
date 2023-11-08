#include <shell.h>
#include <library.h>
#include <pong.h>
#include <stdint.h>
#include <MemoryManagerADT.h>
#include <syscalls.h>
#include <process.h>

#define NULL (void*) 0

extern void invalidOperation();
extern void loopRegisters();

extern uint64_t test_mm(uint64_t argc, char *argv[]);

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
    while(1){
        printf("\n");
        print("$ ", BLUE);
        getInput(input);
        argc = parseInput(input, command, params);
        runModule(command,argc,params);
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
void loadModule(char * name, char * description, void (*function)(void)) {
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
    loadModule("registers", "Prints the values of the processor's registers at the instant they were saved. Registers are saved by pressing Alt", &printRegisters);
    loadModule("divide", "Asks for two numbers and prints the result of dividing one by the other", &divide);
    //loadModule("pong", "Starts the arcade game Pong for two players", &playPong);
    loadModule("clear", "Clears the screen of the shell", &clear);
    loadModule("invop", "Performs an invalid assembly operation (mov cr6, 0x77) and throws an invalid operation exception", &invalidOperation);
    loadModule("testReg", "Sets all registers(except r12, rsp and rbp) at 33h and gives time for pressing 'Alt' and testing the functionality 'registers'", &testRegisters);
    loadModule("mem", "Prints the memory status", &mem);
    loadModule("pid", "Prints the pid of the current process", &printPID);
    loadModule("pstart", "Start a new process given a priority and process name", &pstart);
    loadModule("kill","Kill a process given a PID", &killProcess);
    loadModule("ps","Prints all processes", &printCurrentProcesses);
    loadModule("block","Switch the process status between blocked and ready given a PID", &blockProcess);
    loadModule("nice", "Change the priority of a process given a PID and a priority", &changePriority);
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
            printf(modules[i].description);
        }
    } else if (argc == 1 && argv[0][0] == '2') {
        for(int i=TOTAL_MODULES/2; i<TOTAL_MODULES; i++) {
            printf("\n");
            print(" - ", GREEN);
            print(modules[i].name, GREEN);
            printf(" : ");
            printf(modules[i].description);
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
void printTime() {
    char timeStr[TIME_BUFF_SIZE];
    printf("The current time is ");
    getTime(timeStr);
    printf(timeStr);
}

/**
 * @brief Función correspondiente para el módulo de "registers". Imprime el valor de los registros cuando se
 * guardaron al presionar la tecla Alt. Si no se presionó en ningún momento, imprime un mensaje correspondiente.
 */
void printRegisters() {
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
        printf(buffer);
        printf("h");
    }
}

/**
 * @brief Función correspondiente para el módulo de "divide". Consulta al usuario dos veces para que ingrese
 * dos números: un dividendo y un divisor. Luego, se imprime en pantalla el valor del cociente y el resto al
 * hacer esa división.
 */
void divide() {
    char numberStr[DIV_BUFF_SIZE];
    char divisorStr[DIV_BUFF_SIZE];
    char quotientStr[DIV_BUFF_SIZE];
    char remainderStr[DIV_BUFF_SIZE];
    printf("Insert a number: ");
    getInput(numberStr);
    printf("\n");
    printf("Insert a divisor: ");
    getInput(divisorStr);
    int number = strToNum(numberStr);
    int divisor = strToNum(divisorStr);
    int quotient = number/divisor; 
    int remainder = number%divisor;
    numToStr(quotient, 10, quotientStr);
    numToStr(remainder, 10, remainderStr);
    printf("\n");
    printf("Quotient: ");
    printf(quotientStr);
    printf("\n");
    printf("Remainder: ");
    printf(remainderStr);
}

/**
 * @brief Función correspondiente al módulo de "clear". Limpia la pantalla de la shell.
 */
void clear() {
    enableDoubleBuffer(1);
    enableDoubleBuffer(0);
}

/**
 * @brief Función correspondiente al módulo de "test registers". Imprime un mensaje en pantalla 
 * indicando al usuario que presione alt, y llama a la función loopRegisters, en la cual se setean
 * los registros.
 * 
 */
void testRegisters(){
    printf("Press 'Alt' now! (and wait a few seconds)");
    loopRegisters();
}

void mem() {
    printMemStatus();
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

void printPID() {
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

void printCurrentProcesses() {
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