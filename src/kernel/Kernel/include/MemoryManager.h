#define _MEMORY_MANAGER_H

#include <sys/types.h>
#include <stdint.h>
#include <videodriver.h>
#include <lib.h>

#define NULL (void*) 0
#define false 0
#define true 1

//se usa esto???
typedef struct memStatus {
		size_t free;
		size_t occupied;
} memStatus;


typedef struct MemoryManagerCDT* MemoryManagerADT;
void createMM();
void * allocMemory(size_t size);
int freeMemory(void *data);
void getMemStatus(size_t * free, size_t * occupied);