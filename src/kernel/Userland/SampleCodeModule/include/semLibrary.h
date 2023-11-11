#include <syscalls.h>

sem_type* sem_open(char* name, int value);

int sem_close(sem_type * sem);

int sem_wait(sem_type *sem);

int sem_post(sem_type *sem);

