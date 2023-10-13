/* The Simplest Mememory Manager. Does not permit memory to be freed. */

#include "include/MemoryManagerADT.h"

typedef struct MemoryManagerCDT {
    int size;
    void * nextAddress;
} MemoryManagerCDT;

MemoryManagerADT createMM() {
    MemoryManagerADT mm = (MemoryManagerADT) MEMORY_MANAGER_ADDRESS;
    mm->nextAddress = mm + sizeof(mm);
    mm->size = TOTAL_HEAP_SIZE;
    return mm;
}

void * allocMemory(MemoryManagerADT mm, size_t size) {
    /* 
        mm->nextAddress : dirección del último allocMemory
        mm              : dirección inicial de la memoria
        size            : tamaño de lo que se quiere alocar
        mm->size        : tamaño total de la memoria
    */
    if ((size_t)mm->nextAddress - (size_t)mm + size > mm->size)
        return NULL;
    void * address = mm->nextAddress;
    mm->nextAddress += size;
    return address;
}
