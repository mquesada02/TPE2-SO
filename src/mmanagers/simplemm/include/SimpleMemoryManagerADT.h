#define _SIMPLE_MEMORY_MANAGER_H

#define MEMORY_MANAGER_ADDRESS 4096
#define TOTAL_HEAP_SIZE 4096

#include <stddef.h>

typedef struct MemoryManagerCDT* MemoryManagerADT;

MemoryManagerADT createMM();
void * allocMemory(MemoryManagerADT mm, size_t size);
