#define _MEMORY_MANAGER_H

#include <sys/types.h>
#include <stdint.h>
#include <videodriver.h>
#include <lib.h>

#define NULL 0
#define false 0
#define true 1



typedef struct MemoryManagerCDT* MemoryManagerADT;
void createMM();
void * allocMemory(size_t size);
int freeMemory(void *data);
void getMemStatus(size_t * free, size_t * occupied);