#define _MEMORY_MANAGER_H

#include <sys/types.h>

// | 0x0000000000050000 |0x000000000009FFFF|320 KiB| Free |
// 327679 size
#define HEAP_SIZE 262144 // 2^18
#define HEAP_STARTING_ADDRESS 0x0000000000050000

typedef struct MemoryManagerCDT* MemoryManagerADT;

MemoryManagerADT createMM();

void * allocMemory(MemoryManagerADT mm, size_t size);
void * freeMemory(MemoryManagerADT mm, void * address);

