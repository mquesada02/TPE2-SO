#define _SEMAPHORE_H_
#include <MemoryManager.h>
#include <lib.h>

typedef int sem_type;

int sem_wait(sem_type *sem);

int sem_post(sem_type *sem);

// int sem_init(sem_type *sem, int pshared, unsigned int value);

int sem_close(sem_type *sem);

sem_type* sem_open(char* name);
