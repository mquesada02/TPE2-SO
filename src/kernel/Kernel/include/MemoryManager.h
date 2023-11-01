#define _MEMORY_MANAGER_H

#include <sys/types.h>

#define NULL (void*) 0
#define false 0

typedef struct memStatus {
		size_t free;
		size_t occupied;
} memStatus;

#define MEMORY_MANAGER_SIZE sizeof(MemoryManagerCDT)
#define STRUCTURE_SIZE ((HEAP_SIZE - MEMORY_MANAGER_SIZE) / 4)
#define ALLOC_BLOCK ((HEAP_SIZE - MEMORY_MANAGER_SIZE) * 3 / 4)
// | 0x0000000000050000 |0x000000000009FFFF|320 KiB| Free |
// 327679 size
#define HEAP_SIZE 262144 // 2^18
#define HEAP_STARTING_ADDRESS 0x0000000000050000

typedef struct Node * FreeList;

typedef struct MemoryManagerCDT* MemoryManagerADT;
void createMM();
void * allocMemory(size_t size);
int freeMemory(void *data);
void getMemStatus(size_t * free, size_t * occupied);