#include <semLibrary.h>

sem_type* sem_open(char* name, int value){
    return syscall_sem_open(name, value);
}

int sem_close(sem_type * sem){
    return syscall_sem_close(sem);
}

int sem_wait(sem_type *sem){
    return syscall_sem_wait(sem);
}

int sem_post(sem_type *sem){
    return syscall_sem_post(sem);
}

