#define _MEMORY_MANAGER_H

#include <stdlib.h>
#include <sys/types.h>

#define HEAP_STARTING_ADDRESS malloc(2048)/* to define */
#define HEAP_END_ADDRESS HEAP_STARTING_ADDRESS+2048/* to define */

typedef struct MemoryManagerCDT* MemoryManagerADT;

MemoryManagerADT createMM();
void * allocMemory(MemoryManagerADT mm, size_t size);
void * freeMemory(MemoryManagerADT mm, void * address);

