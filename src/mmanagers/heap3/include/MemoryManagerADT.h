#define _MEMORY_MANAGER_H

#include <stdlib.h>
#include <sys/types.h>

#define HEAP_SIZE 2048
#define HEAP_STARTING_ADDRESS malloc(HEAP_SIZE)

typedef struct MemoryManagerCDT* MemoryManagerADT;

MemoryManagerADT createMM();

void * allocMemory(MemoryManagerADT mm, size_t size);
int freeMemory(MemoryManagerADT mm, void *data);

