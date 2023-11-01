//importante -> cambiar malloc y free por los nuestros, pero estan en userspace asi que no se :/
#include <sys/types.h>

//extern void * allocMemory(size_t size);
//extern int freeMemory(void *data);

#define NULL 0

#define maxQuantums 4

#define prioritiesAmount 15
static int initialPriority = 0;

enum state {running, ready, blocked, exited};

#define priorityLevels 5

typedef struct Node {
    unsigned long rsp;
    enum state state;
    int quantums;
    struct Node *next;
} Node;

typedef struct Queue {
    Node *first;
    Node *last;
} Queue;

Queue priorityQueue[5];
static int priorityCounter = 0;

void initPriorityQueue(){
    for (int i = 0; i < priorityLevels; i++){
        priorityQueue[i].first = NULL;
        priorityQueue[i].last = NULL;
    }
}

void addProcess(int priority, unsigned long rsp){
    Node *newNode;// = (Node *) malloc(sizeof(Node));
    newNode->rsp = rsp;
    newNode->state = ready;
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

void stopProcess(unsigned long rsp, int state){
    int i = 0;
    while(i < priorityLevels){
        if(!priorityQueue[priorityCounter].first){
            priorityCounter = (priorityCounter + 1) % priorityLevels;
            i++;
            continue;
        }
        Node *aux = priorityQueue[priorityCounter].first;
        Node *prev = priorityQueue[priorityCounter].last;
        while(aux->rsp != rsp && aux != priorityQueue[priorityCounter].last){
            aux = aux->next;
            prev = prev->next;
        }
        if(aux->rsp == rsp){
            aux->state = state;
            if(aux->quantums == maxQuantums){
                aux->quantums = 0;
                //si estaba en el ultimo nivel de prioridad no hace nada
                if(priorityCounter == 4) return;
                //sino lo saca de ese nivel
                if(priorityQueue[priorityCounter].first == priorityQueue[priorityCounter].last){
                    priorityQueue[priorityCounter].first = NULL;
                    priorityQueue[priorityCounter].last = NULL;
                } else {
                    prev->next = aux->next;
                    if(aux == priorityQueue[priorityCounter].first || aux == priorityQueue[priorityCounter].last){
                        priorityQueue[priorityCounter].first = aux->next;
                        priorityQueue[priorityCounter].last = prev;
                    }
                }
                //y lo agrega al siguiente
                priorityQueue[priorityCounter + 1].last->next = aux;
                priorityQueue[priorityCounter + 1].last = aux;
                aux->next = priorityQueue[priorityCounter + 1].first;
            }
            else {
            priorityCounter = (priorityCounter + 1) % priorityLevels;
            i++;
            }
        }
    }
}

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
        }
        priorityQueue[priorityCounter].first->state = running;
        priorityQueue[priorityCounter].first->quantums++;
        return priorityQueue[priorityCounter].first->rsp;
    }
    //si salio de aca es porque no encontro nungun proceso ready para correr
    return NULL; /*stack pointer del proceso halt ******************************************************************************************************************************************************/
}

unsigned long schedule(unsigned long rsp){
    unsigned long toReturn = selectToRun();
    stopProcess(rsp, ready);
    return toReturn;
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