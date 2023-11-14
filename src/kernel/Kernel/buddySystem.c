#ifdef BUDDYSYSTEM
#include <MemoryManager.h>

#define true 1
#define false 0

#define HEAP_SIZE MAX_ALLOC + 2* LISTS_SIZE
#define HEAP_STARTING_ADDRESS 0x0000000000050000

#define MAX_ALLOC 131072 // 2^17
#define MAX_ALLOC_LOG2 17
#define MIN_ALLOC 16 // 2^4
#define MIN_ALLOC_LOG2 4
#define LISTS_SIZE MAX_ALLOC / MIN_ALLOC * sizeof(Node) //

#define NULL (void *) 0

typedef struct pair{
    unsigned int lb;
    unsigned int ub;
} pair;

typedef struct Node{
    pair mem;
    struct Node *next; 
} Node;

typedef Node* List;

typedef struct BuddyStruct{
    List freeList[MAX_ALLOC_LOG2];
    List occupiedList;
    uint64_t free;
    uint64_t occupied;
    void * dataMemory;
} BuddyStruct;

typedef BuddyStruct * BuddySystem;

void* currentAddress = (void *) (HEAP_STARTING_ADDRESS+sizeof(BuddyStruct));
static void* lastPhysicalAddress = NULL;

static BuddySystem bs = NULL; 

void * allocFirst(size_t size) {
    void * aux = currentAddress;
    currentAddress += size;
    return aux;
}

void createMM(){
    bs = (BuddySystem) HEAP_STARTING_ADDRESS;
    bs->occupiedList = NULL;
    //los nodos de todas las listas se van a ir creando en el mismo lugar fisico
    lastPhysicalAddress = allocFirst(LISTS_SIZE - sizeof(BuddyStruct));
    bs->dataMemory = allocFirst(MAX_ALLOC);
    bs->free = MAX_ALLOC;
    bs->occupied = 0;
    for (int i =  0; i < MAX_ALLOC_LOG2  ; i++){
        bs->freeList[i] = NULL; 
    }
    //inicialmente solo esta libre el bloque mas grande
    nNode(&(bs->freeList[MAX_ALLOC_LOG2 - 1]), 0, MAX_ALLOC-1);
}

void nNode(List *list, unsigned int lb, unsigned int ub){
    Node * newNode = (Node *) lastPhysicalAddress;
    newNode->mem.ub = ub;
    newNode->mem.lb = lb;
    newNode->next = *list;
    *list = newNode;
    lastPhysicalAddress += sizeof(Node);
}

unsigned int Log2rec(unsigned int n){
    return (n > 1) ? 1 + Log2rec(n / 2) : 0;
}

unsigned int pow(unsigned int base, unsigned int exp){
    unsigned int res = 1;
    for (int i = 0; i < exp; i++){
        res *= base;
    }
    return res;
}

unsigned int Log2(unsigned int n){
    unsigned int res = Log2rec(n);
    return pow(2, res) == n ? res : res + 1;
}

void * allocMemory(size_t size){
    int x = Log2(size); // me dice en que lugar del array de listas tendria que ubicarlo
    x = x < MIN_ALLOC_LOG2 ? MIN_ALLOC_LOG2 : x;

    List aux;

    //ya hay un bloque del tamaño correspondiente
    if(bs->freeList[x] != NULL){
        aux = bs->freeList[x];
        aux->next = bs->occupiedList;
        bs->occupiedList = aux;
        bs->freeList[x] = bs->freeList[x]->next;
        bs->occupied += pow(2, x);
        bs->free -= pow(2, x);
        return bs->dataMemory + aux->mem.lb;
    }

    int i;
    //sino busco un bloque mas grande
    for (i = x + 1; i < MAX_ALLOC_LOG2; i++){
        if(bs->freeList[i] != NULL){
            break;
        }
    }

    //no se encontro un bloque con espacio suficiente -> no se pudo alocar memoria
    if(bs->freeList[i] == NULL){
        return NULL;
    }

    //se encontro un bloque mas grande -> lo divido en dos
    aux = bs->freeList[i];
    bs->freeList[i] = bs->freeList[i]->next;

    i--;
    for(; i >= x; i--){
        nNode(&bs->freeList[i], aux->mem.lb + (aux->mem.ub - aux->mem.lb + 1)/2, aux->mem.ub);
        aux->mem.ub = aux->mem.lb + (aux->mem.ub - aux->mem.lb)/2;
    }
    nNode(&bs->occupiedList, aux->mem.lb, aux->mem.ub);
    bs->occupied += pow(2, x);
    bs->free -= pow(2, x);
    return bs->dataMemory + aux->mem.lb;
}

int freeMemory(void * data){
    int offset = data - bs->dataMemory;

    List occupiedAux = bs->occupiedList;
    List occupiedPrev = NULL;
    while(occupiedAux != NULL && occupiedAux->mem.lb != offset){
        occupiedPrev = occupiedAux;
        occupiedAux = occupiedAux->next;
    }
    //si el espacio que se pidio liberar no es valido
    if(occupiedAux == NULL){
        return false;
    }
    int size = occupiedAux->mem.ub - occupiedAux->mem.lb + 1;
    bs->occupied -= size;
    bs->free += size;
    //si el espacio a liberar estaba primero en la lista de ocupados
    if(occupiedPrev==NULL){
        bs->occupiedList = bs->occupiedList->next;
    }
    else{
        occupiedPrev->next = occupiedAux->next;
    }

    //busco la lista de libres correspondiente
    int x = Log2(occupiedAux->mem.ub - occupiedAux->mem.lb + 1);

    //calculo el buddyNumber y buddyAddress
    int buddyNumber = offset / size;
    int buddyAddress;
    if(buddyNumber % 2 == 0){
        buddyAddress = offset + pow(2, x);
    }
    else{
        buddyAddress = offset - pow(2, x);
    }

    List freeAux;
    List freePrev;
    while(x < MAX_ALLOC_LOG2){
        //busco en la lista si esta el buddy
        freeAux = bs->freeList[x];
        freePrev = NULL;
        while (freeAux != NULL && freeAux->mem.lb != buddyAddress){
            freePrev = freeAux;
            freeAux = freeAux->next;
        }
        //si el buddy tambien estaba libre
        if(freeAux!=NULL){
            //lo saco de la lista en la que estaba
            if(freePrev==NULL){
                bs->freeList[x] = bs->freeList[x]->next;
            }
            else{
                freePrev->next = freeAux->next;
            }

            //el buddy es el mayor de los dos
            if(buddyNumber % 2 == 0){
                freeAux->mem.lb -= pow(2, x);
            }
            //el buddy es el menor de los dos
            else{
                freeAux->mem.ub += pow(2, x);
            }

            //ahora tengo que chequear si el nuevo nodo que se creo tambien tiene un buddy libre
            //calculo el buddyNumber y buddyAddress
            buddyNumber = freeAux->mem.lb / (freeAux->mem.ub - freeAux->mem.lb + 1);
            if(buddyNumber % 2 == 0){
                buddyAddress = freeAux->mem.lb + pow(2, x);
            }
            else{
                buddyAddress = freeAux->mem.lb - pow(2, x);
            }
            x++;
            //uso occupiedAux en el else para que valga tambien para el primer caso
            occupiedAux = freeAux;
        }
        //si el buddy no estaba libre (en algun momento va a pasar si o si)
        else{
            occupiedAux->next = bs->freeList[x];
            bs->freeList[x] = occupiedAux;
            return true;
        }
    }
}

void getMemStatus(size_t * free, size_t * occupied) {
    *free = bs->free;
    *occupied = bs->occupied;
}
#endif
