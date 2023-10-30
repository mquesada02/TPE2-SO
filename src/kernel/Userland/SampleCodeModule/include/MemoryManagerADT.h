#define _MEMORY_MANAGER_H

#include <sys/types.h>

typedef struct MemoryManagerCDT* MemoryManagerADT;

void createMM();

void * allocMemory(size_t size);
int freeMemory(void *data);

