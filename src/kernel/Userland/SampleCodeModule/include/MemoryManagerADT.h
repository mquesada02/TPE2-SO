#define _MEMORY_MANAGER_H

#include <sys/types.h>

typedef struct MemoryManagerCDT* MemoryManagerADT;

void * allocMemory(size_t size);
int freeMemory(void *data);
void printMemStatus();

