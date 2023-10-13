#define _MEMORY_MANAGER_H
// | 0x0000000000050000 |0x000000000009FFFF|320 KiB| Free |
#define MEMORY_MANAGER_ADDRESS 0x0000000000050000 
#define TOTAL_HEAP_SIZE 0x000000000004FFFF

#include <stddef.h>

typedef struct MemoryManagerCDT* MemoryManagerADT;

MemoryManagerADT createMM();
void * allocMemory(MemoryManagerADT mm, size_t size);
