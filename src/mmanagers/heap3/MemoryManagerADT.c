#include <MemoryManagerADT.h>
#include "./dsa/include/FreeList.h"

void * currentAddress = NULL;

MemoryManagerADT createMM() {
    
}

void * allocFirst(size_t size) {
    if (currentAddress == NULL) {
        currentAddress = HEAP_STARTING_ADDRESS;
    }
    void * aux = currentAddress;
    currentAddress += size;
    return aux;
}
void * allocMemory(MemoryManagerADT mm, size_t size) {

}
void * freeMemory(MemoryManagerADT mm, void * address) {

}