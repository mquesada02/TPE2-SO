#include <semaphore.h>

#define NULL (void *) 0
#define MAX_LENGTH 30
#define MAX_SEMS 10

typedef int sem_type;

typedef struct Node{
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
    aux->uses = 1;
    aux->next = NULL;
    return aux;
}

sem_type* sem_open(char* name){
    if(sems == NULL){
        sems = (sem_type *) allocMemory(MAX_SEMS * sizeof(sem_type));
        for (int i = 0; i < MAX_SEMS; i++){
            sems[i] = 0;
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
    while (freeSpot < MAX_SEMS && sems[freeSpot] != 0){
        freeSpot++;
    }
    if(freeSpot == MAX_SEMS){
        return NULL;
    }
    aux = newNode(name, &sems[freeSpot]);
    aux->next = semList;
    semList = aux;
    return aux->sem;
}

int sem_close(char* name){
    Node * aux = semList;
    Node * prev = NULL;
    while (aux != NULL){
        if (strcmplen(aux->name, name, MAX_LENGTH) == 0){
            aux->uses--;
            if (aux->uses == 0){
                if (prev == NULL){
                    semList = aux->next;
                } else {
                    prev->next = aux->next;
                }
                *(aux->sem) = 0;
                freeMemory(aux);
            }
            return 0;
        }
        prev = aux;
        aux = aux->next;
    }
    return -1;
}



