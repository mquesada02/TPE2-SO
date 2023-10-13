#ifndef _SIMPLE_MEMORY_MANAGER_ADT_H
#define _SIMPLE_MEMORY_MANAGER_ADT_H

#include <stdlib.h>
// | 0x0000000000050000 |0x000000000009FFFF|320 KiB| Free |
#define TOTAL_HEAP_SIZE 0x000000000004FFFF
#define MEMORY_MANAGER_ADDRESS malloc(TOTAL_HEAP_SIZE)


#include <stddef.h>

typedef struct MemoryManagerCDT* MemoryManagerADT;

MemoryManagerADT createMM();
void * allocMemory(MemoryManagerADT mm, size_t size);

#endif