#include <MemoryManager.h>
#include <stdint.h>
#include <videodriver.h>


#define NULL (void*) 0

#define MEMORY_MANAGER_SIZE sizeof(MemoryManagerCDT)
#define STRUCTURE_SIZE ((HEAP_SIZE - MEMORY_MANAGER_SIZE) / 4)
#define ALLOC_BLOCK ((HEAP_SIZE - MEMORY_MANAGER_SIZE) * 3 / 4)

#define true 1
#define false 0

typedef struct Node * FreeList;

typedef struct Node {
    void * data; // address of the alloc data
    size_t size; // size of data in the alloc blocks
    char occupied;
	int present;
    FreeList prev;
    FreeList next;
} Node;

void * insert(size_t size);
size_t delete(void *data);
void moveLastPhysicalAddress();

typedef struct MemoryManagerCDT {
    uint64_t free;
    uint64_t occupied;
    FreeList root;
    void * dataMemory;
} MemoryManagerCDT;

void * currentAddress = ((void *)(HEAP_STARTING_ADDRESS + MEMORY_MANAGER_SIZE));

FreeList lastPhysicalAddress = NULL;
FreeList firstPhysicalAddress = NULL;

static MemoryManagerADT mm = NULL;

void * allocFirst(size_t size) {
    void * aux = currentAddress;
    currentAddress += size;
    return aux;
}

void initList() {
    
	mm->root			= (FreeList) allocFirst(STRUCTURE_SIZE);
	mm->dataMemory		= allocFirst(ALLOC_BLOCK);
	mm->root->data		= mm->dataMemory;
	mm->root->size		= ALLOC_BLOCK;
	mm->root->occupied	= false;
	mm->root->present	= true;
	mm->root->prev		= NULL;
	mm->root->next		= NULL;

	lastPhysicalAddress = mm->root;
	firstPhysicalAddress = mm->root;
}

void createMM() {
    mm = (MemoryManagerADT) HEAP_STARTING_ADDRESS;
	initList();
    mm->free = ALLOC_BLOCK;
    mm->occupied = 0;
}


void * allocMemory(size_t size) {
    if (size <= 0 || mm == NULL) {
        return NULL;
    }
    void * address = insert(size);
    if (address != NULL) {
        mm->free -= size;
        mm->occupied += size;
    }
    return address;
}

int freeMemory(void *data) {
	if (mm == NULL)
		return false;
    size_t size = delete(data);

    if (size == 0){
        return false;	
    }
    mm->free += size;
    mm->occupied -= size;
    return true;
}

/**
 * @brief Almacena lógicamente sobre la FreeList un Node de tamaño size solicitado, y retorna el puntero a la dirección del espacio solicitado.
 * 
 * @param size Tamaño del bloque solicitado
 * @return void * Puntero a la dirección del espacio alocado.
 */
void * insert(size_t size) {
	if (size == 0)
		return NULL;
	FreeList current = mm->root;
	/* iterate until you find a block where (the data is null and the size
	 * needed fits) or when the next is null */
	while (!((current->occupied == false && current->size >= size) ||
			 current->next == NULL)) {
		current = current->next;
	}
	if (current->next == NULL && current->size < size)
		return NULL;
	if (current->occupied == false) {
		current->occupied = true;
        if(current->size == size)
            return current->data;
		lastPhysicalAddress += sizeof(Node);
		FreeList emptyRemaining	 = lastPhysicalAddress;
        emptyRemaining->next     = current->next;
		current->next			 = emptyRemaining;
        emptyRemaining->prev	 = current;
		emptyRemaining->size	 = current->size - size;
		emptyRemaining->data	 = current->data + size;
		emptyRemaining->occupied = false;
		emptyRemaining->present  = true;
		current->size = size;
		
		}
	
	return current->data;
}


/**
 * @brief 
 * 
 * @param data 
 * @return * int Devuelve 1 si libero correctamente la memoria y 0 si no encontro la memoria a liberar
 */
size_t delete(void *data) {
    size_t toReturn = 0;
	FreeList current = mm->root;
	
	while (current->data != data && current->next != NULL) {
		current = current->next;
	}
	if (current->data != data) return toReturn;

	//liberar el espacio
    toReturn = current->size;

	//si tanto el anterior como el siguiente apuntan a espacio ocupado es solo marcar como desocupado
	//if ((current->prev->occupied && current->next->occupied) || current == mm->root) {
		current->occupied = false;
	//} 

	if (current == mm->root) 
		return toReturn;

    if (!current->prev->occupied) {
		//unicamente el anterior apunta a espacio libre
		if(current->next->occupied){
			
			current->prev->size += current->size;
			current->prev->next = current->next;

			current->present = false;
			if(current == lastPhysicalAddress){
				moveLastPhysicalAddress();
			}
			//moveLastPhysicalAddress(current);
		}		
        //si tanto el anterior como el siguiente apuntaban a espacios libres
        else{
		    current->prev->size += current->size + current->next->size;
		    current->prev->next = current->next->next;

			current->next->present = false;
			current->present = false;
			if(current->next == lastPhysicalAddress || current == lastPhysicalAddress){
				moveLastPhysicalAddress();
			}
		    //moveLastPhysicalAddress(current->next);
		    //moveLastPhysicalAddress(current);
        }
	}
    
	// sizes en 0 en los últimos bloques
	// llega al final de la lista con size en 0 por alguna razón
	// se pisa dataMemory

	//unicamente el siguiente apunta a espacio libre
	else if (current->next->occupied){
		current->occupied = false;
		current->size += current->next->size;
		current->next = current->next->next;

		current->next->present = false;
		if(current->next == lastPhysicalAddress){
			moveLastPhysicalAddress();
		}
		//moveLastPhysicalAddress(current->next);
	}
    
	if (current == lastPhysicalAddress) {
		while (lastPhysicalAddress->occupied == false && lastPhysicalAddress->prev != NULL) {
			lastPhysicalAddress = lastPhysicalAddress->prev;
		}
		if (lastPhysicalAddress->prev == NULL)
			lastPhysicalAddress = mm->root;
		else
			lastPhysicalAddress += sizeof(Node);
		
	}

    return toReturn;
}

void moveLastPhysicalAddress(){
	while(!lastPhysicalAddress->present && lastPhysicalAddress != firstPhysicalAddress){
		lastPhysicalAddress -= sizeof(Node);
	}
}
/*
void moveLastPhysicalAddress(FreeList node) {
	node->data = lastPhysicalAddress->data;
	node->size = lastPhysicalAddress->size;
	node->occupied = lastPhysicalAddress->occupied;
	node->prev = lastPhysicalAddress->prev;
	node->next = lastPhysicalAddress->next;

	node->prev->next = node;
	lastPhysicalAddress -= sizeof(Node); 
}
*/

void getMemStatus(size_t * free, size_t * occupied) {
    *free = mm->free;
    *occupied = mm->occupied;
}