// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "./include/semaphore.h"

#define MAX_LENGTH 30
#define MAX_SEMS 25

typedef int sem_type;

extern void sem_lock_wait(char * lock, sem_type * sem);
extern void sem_lock_post(char * lock, sem_type * sem);

typedef struct Node{    
    char lock;
    char name[MAX_LENGTH];
    sem_type * sem;
    int uses;
    struct Node * next;
} Node;

typedef Node * List;

List semList = NULL;
sem_type *sems = NULL;

Node * newNode(char * name, sem_type * sem){
    Node * aux = (Node *) allocMemory(sizeof(Node));
    strcpy(aux->name, name, MAX_LENGTH);
    aux->sem = sem;
    aux->lock = 1;
    aux->uses = 1;
    aux->next = NULL;
    return aux;
}

sem_type* sem_open(char* name, int value){
    if(sems == NULL){
        sems = (sem_type *) allocMemory(MAX_SEMS * sizeof(sem_type));
        for (int i = 0; i < MAX_SEMS; i++){
            sems[i] = -1;
        }
    }
    if (semList == NULL){
        semList = newNode(name, &sems[0]);
        return semList->sem;
    }
    Node * aux = semList;
    while (aux != NULL){
        if (strcmplen(aux->name, name, MAX_LENGTH) == 0){
            aux->uses++;
            return aux->sem;
        }
        aux = aux->next;
    }
    int freeSpot = 0;
    while (freeSpot < MAX_SEMS && sems[freeSpot] != -1){
        freeSpot++;
    }
    if(freeSpot == MAX_SEMS){
        return NULL;
    }
    sems[freeSpot] = value;
    aux = newNode(name, &sems[freeSpot]);
    aux->next = semList;
    semList = aux;
    return aux->sem;
}

int sem_close(sem_type* sem){
    Node * aux = semList;
    Node * prev = NULL;
    while (aux != NULL){
        if (aux->sem == sem){
            aux->uses--;
            if (aux->uses == 0){
                if (prev == NULL){
                    semList = aux->next;
                } else {
                    prev->next = aux->next;
                }
                *(aux->sem) = -1;
                freeMemory(aux);
            }
            return 0;
        }
        prev = aux;
        aux = aux->next;
    }
    return -1;
}
// int sem_close(char* name){
//     Node * aux = semList;
//     Node * prev = NULL;
//     while (aux != NULL){
//         if (strcmplen(aux->name, name, MAX_LENGTH) == 0){
//             aux->uses--;
//             if (aux->uses == 0){
//                 if (prev == NULL){
//                     semList = aux->next;
//                 } else {
//                     prev->next = aux->next;
//                 }
//                 *(aux->sem) = 0;
//                 freeMemory(aux);
//             }
//             return 0;
//         }
//         prev = aux;
//         aux = aux->next;
//     }
//     return -1;
// }

int sem_wait(sem_type *sem){
    Node* aux = semList;
    while(aux != NULL && aux->sem != sem){
        aux = aux->next;
    }
    if(aux == NULL){
        return -1;
    }
    sem_lock_wait(&(aux->lock), aux->sem);
    if(*sem == 0){
        sem_lock_post(&(aux->lock), aux->sem);
        blockSemProcess(sem, 0);
    }
    //si sigo es porque se desbloqueo el proceso
    //si se llamo a un unblock process es porque se tenia acceso exclusivo al semaforo, 
    //asi que simplemente no libero ese acceso para que cualquier otro proceso que no sea
    //el que se acaba de desboquear no pueda modificar el semaforo
    (*sem)--;
    sem_lock_post(&(aux->lock), aux->sem);
    return 0;    
}

int sem_post(sem_type *sem){
    Node* aux = semList;
    while(aux != NULL && aux->sem != sem){
        aux = aux->next;
    }
    if(aux == NULL){
        return -1;
    }
    sem_lock_wait(&(aux->lock), aux->sem);
    (*sem)++;
    if(unblockSemProcess(sem, 0)){
        //no hago un sem_lock_post porque el semaforo lo esta accediendo ahora el proceso que se desbloqueo
        return 0;
    }
    //si no habia nungun proceso esperando a ese semaforo
    sem_lock_post(&(aux->lock), aux->sem);
    return 0; 
}

