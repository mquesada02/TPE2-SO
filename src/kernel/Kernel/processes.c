#include <processes.h>
#include <interrupts.h>
#include <MemoryManager.h>

#define PAGE_SIZE 4096
#define HALT_PAGE 256


extern unsigned long prepare_process(void * memory, void (* process), char argc, char* argv[]);
extern void halting(void);
extern void addProcess(int priority, unsigned long rsp);

void startProcess(int priority, void (* process), char argc, char* argv[]) {
    //_cli();
    void * processMemory = allocMemory(PAGE_SIZE);
    unsigned long rsp = prepare_process(processMemory + PAGE_SIZE, process, argc, argv);
    // int 0x20
    addProcess(priority, rsp);
    //_sti();
}

unsigned long prepareHalt() {
    void * haltMemory = allocMemory(HALT_PAGE);
    return prepare_process(haltMemory + HALT_PAGE, &halting, 0, NULL);
}

void halting() {
    while(1) {
        _hlt();
    }
}
