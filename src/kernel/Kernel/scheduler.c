//importante -> cambiar malloc y free por los nuestros, pero estan en userspace asi que no se :/
#include <sys/types.h>
#include <MemoryManager.h>
#include <processes.h>
#include <scheduler.h>

#define NULL 0

#define maxQuantums 4

#define prioritiesAmount 15
static int initialPriority = 0;

static size_t runningPID = 15;

static size_t foregroundPID = 1; // shell pid by default
#define HALT_PID 0

#define priorityLevels 5

typedef struct Node {
    unsigned long rsp;
    size_t pid;
    enum state state;
    char foreground;
    int quantums;
    struct Node *next;
} Node;

typedef struct Queue {
    Node *first;
    Node *last;
} Queue;

Queue * priorityQueue;
static int priorityCounter = 0;
unsigned long haltRSP = NULL;

void initPriorityQueue(){
    priorityQueue = (Queue *) allocMemory(sizeof(Queue) * priorityLevels);
    for (int i = 0; i < priorityLevels; i++){
        priorityQueue[i].first = NULL;
        priorityQueue[i].last = NULL;
    }
    haltRSP = prepareHalt();
}

void addProcess(int priority, unsigned long rsp, size_t pid, char foreground){
    Node *newNode = (Node *) allocMemory(sizeof(Node));
    newNode->rsp = rsp;
    newNode->pid = pid;
    newNode->state = ready;
    newNode->foreground = foreground;
    newNode->quantums = 0;
    newNode->next = NULL;
    if (priorityQueue[priority].first == NULL){
        priorityQueue[priority].first = newNode;
        priorityQueue[priority].last = newNode;
        newNode->next = newNode;
    } else {
        priorityQueue[priority].last->next = newNode;
        priorityQueue[priority].last = newNode;
        newNode->next = priorityQueue[priority].first;
    }
}

/* void stopProcess(unsigned long rsp, int state){
    int i = 0;
    while(i < priorityLevels) {
        if(!priorityQueue[priorityCounter].first){
            priorityCounter = (priorityCounter + 1) % priorityLevels;
            i++;
            continue;
        }
        Node *aux = priorityQueue[priorityCounter].first;
        Node *prev = priorityQueue[priorityCounter].last;
        while(aux->pid != runningPID && aux != priorityQueue[priorityCounter].last){
            aux = aux->next;
            prev = prev->next;
        }
        if(aux->pid == runningPID){
            aux->state = state;
            aux->rsp = rsp; // exchange rsp
            if (aux->pid == HALT_PID)
                haltRSP = rsp;
            if(aux->quantums == maxQuantums ){
                aux->quantums = 0;
                //si estaba en el ultimo nivel de prioridad no hace nada
                if(priorityCounter == 4) return;
                //sino lo saca de ese nivel
                if(priorityQueue[priorityCounter].first == priorityQueue[priorityCounter].last){
                    priorityQueue[priorityCounter].first = NULL;
                    priorityQueue[priorityCounter].last = NULL;
                } else {
                    if (aux == priorityQueue[priorityCounter].first) {
                        priorityQueue[priorityCounter].first = aux->next;
                        priorityQueue[priorityCounter].last->next = aux->next;
                    }
                    if (aux == priorityQueue[priorityCounter].last) {
                        priorityQueue[priorityCounter].last = prev;
                        prev->next = aux->next;
                    }
                    prev->next = aux->next;
                    if(aux == priorityQueue[priorityCounter].first || aux == priorityQueue[priorityCounter].last){
                        priorityQueue[priorityCounter].first = aux->next;
                        priorityQueue[priorityCounter].last = prev;
                    }
                }
                //y lo agrega al siguiente
                if (priorityQueue[(priorityCounter + 1) % priorityLevels].first == NULL) {
                    priorityQueue[(priorityCounter + 1) % priorityLevels].first = aux;
                    priorityQueue[(priorityCounter + 1) % priorityLevels].last = aux;
                    aux->next = aux;
                } else {
                    priorityQueue[(priorityCounter + 1) % priorityLevels].last->next = aux;
                    aux->next = priorityQueue[(priorityCounter + 1) % priorityLevels].first;
                }
            }
            else {
                priorityCounter = (priorityCounter + 1) % priorityLevels;
            //i++;
            }
        }
        i++;
    }
} */

void stopProcess(unsigned long rsp, int state) {
    for(int i=0;i<priorityLevels;i++) {
        Node * current = priorityQueue[i].first;
        Node * prev = priorityQueue[i].last;
        Node * last = priorityQueue[i].last;
        while (current->pid != runningPID && current != last) {
            current = current->next;
            prev = prev->next;
        }
        if (current->pid == runningPID) {
            current->state = state;
            if (rsp != 0) { // para aprovecharlo para blockRunning
                current->rsp = rsp;
                if (current->pid == HALT_PID)
                    haltRSP = rsp;
            }
            
            /* if (current->quantums == maxQuantums) {
                current->quantums = 0;
                if (i == priorityLevels - 1) return;
                // sino lo saco de la lista y lo agrego en la siguiente prioridad
                // si es el único en la lista dejo en null
                // sino me fijo si es el primero o el último
                if (priorityQueue[i].first == priorityQueue[i].last) {
                    priorityQueue[i].first = NULL;
                    priorityQueue[i].last = NULL;
                } else {
                    if (current == priorityQueue[i].first) {
                        priorityQueue[i].first = current->next;
                        priorityQueue[i].last->next = current->next;
                    } else if (current == priorityQueue[i].last) {
                        priorityQueue[i].last = prev;
                        prev->next = current->next;
                    } else {
                        // está "entre medio" del primero y el último
                        prev->next = current->next;
                    }
                }
                // y lo agrego al siguiente
                if (priorityQueue[(i + 1) % priorityLevels].first == NULL) {
                    priorityQueue[(i + 1) % priorityLevels].first = current;
                    priorityQueue[(i + 1) % priorityLevels].last = current;
                    current->next = current;
                } else {
                    priorityQueue[(i + 1) % priorityLevels].last->next = current;
                    current->next = priorityQueue[(i + 1) % priorityLevels].first;
                }   
            } */
            return;
        }
    }
}
/* 
void blockRunningProcess() {
    stopProcess(0, blocked);
}

void unblockForegroundProcess(char reason) {
    if (reason == 'r') {
        if (isBlocked(foregroundPID))
            unblockForeground();
    }
}

void unblockForeground() {
    for(int i=0;i<priorityLevels;i++) {
        Node * current = priorityQueue[i].first;
        Node * last = priorityQueue[i].last;
        while (current->pid != foregroundPID && current != last) {
            current = current->next;
        }
        if (current->pid == foregroundPID) {
            current->state = ready;
            return;
        }
    }
} */

unsigned long selectToRun(){
    setPriorityCounter();
    initialPriority = (initialPriority + 1) % prioritiesAmount;
    int i = 0;
    while(i < priorityLevels){
        if(!priorityQueue[priorityCounter].first){
            priorityCounter = (priorityCounter + 1) % priorityLevels;
            i++;
            continue;
        }
        Node *aux = priorityQueue[priorityCounter].last;
        while(priorityQueue[priorityCounter].first->state != ready && priorityQueue[priorityCounter].first != aux){
            priorityQueue[priorityCounter].last = priorityQueue[priorityCounter].first;
            priorityQueue[priorityCounter].first = priorityQueue[priorityCounter].first->next;
        }
        if(priorityQueue[priorityCounter].first->state != ready){
            priorityCounter = (priorityCounter + 1) % priorityLevels;
            i++;
            continue;
        } else {
            priorityQueue[priorityCounter].first->state = running;
            runningPID = priorityQueue[priorityCounter].first->pid;
            if (priorityQueue[priorityCounter].first->foreground)
                foregroundPID = runningPID;

            priorityQueue[priorityCounter].first->quantums++;
            return priorityQueue[priorityCounter].first->rsp;
        }
        
    }
    runningPID = HALT_PID;
    //si salio de aca es porque no encontro nungun proceso ready para correr
    return haltRSP;
}

void setProcessStatePQ(size_t pid, int state) {
    for(int i=0;i<priorityLevels;i++) {
        Node * current = priorityQueue[i].first;
        Node * last = priorityQueue[i].last;
        while (current->pid != pid && current != last) {
            current = current->next;
        }
        if (current->pid == pid) {
            current->state = state;
            return;
        }
    }

}

unsigned long schedule(unsigned long rsp){
    stopProcess(rsp, getProcessState(runningPID));
    unsigned long toReturn = selectToRun();
    return toReturn;
}

size_t getRunningPID() {
    return runningPID;
}

size_t getForegroundPID() {
    return foregroundPID;
}

//determina desde que nivel de prioridad arranca el scheduler a buscar un proceso, y dependiendo de la prioridad arranca mas veces por ese
void setPriorityCounter(){
    switch(initialPriority){
        case 0:
        case 5:
        case 9:
        case 12:
        case 14:
            priorityCounter = 0;
            break;
        case 1:
        case 6:
        case 10:
        case 13:
            priorityCounter = 1;
            break;
        case 2:
        case 7:
        case 11:
            priorityCounter = 2;
            break;
        case 3:
        case 8:
            priorityCounter = 3;
            break;
        case 4:
            priorityCounter = 4;
            break;
    }
}