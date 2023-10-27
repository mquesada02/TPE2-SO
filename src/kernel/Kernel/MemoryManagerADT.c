#include <MemoryManagerADT.h>

#define MEMORY_MANAGER_SIZE sizeof(MemoryManagerADT)
#define STRUCTURE_SIZE ((HEAP_SIZE - MEMORY_MANAGER_SIZE) / 4)
#define ALLOC_BLOCK ((HEAP_SIZE - MEMORY_MANAGER_SIZE) * 3 / 4)

#define true 1
#define false 0

void initList(MemoryManagerADT mm);
void * allocFirst(size_t size);

void * insert(MemoryManagerADT mm, size_t size);
int delete(MemoryManager mm, void *data);
void moveLastPhysicalAddress(FreeList node);

void * currentAddress = NULL;
void *lastPhysicalAddress = NULL;

typedef struct MemoryManagerCDT {
    uint64_t free;
    uint64_t occupied;
    FreeList root;
    void * dataMemory;
} MemoryManagerCDT;

typedef struct Node {
    void * data; // address of the alloc data
    size_t size; // size of data in the alloc blocks
    char ocuppied;
    FreeList prev;
    FreeList next;
} Node;

typedef Node * FreeList;

void * allocFirst(size_t size) {
    if (currentAddress == NULL) {
        currentAddress = HEAP_STARTING_ADDRESS + MEMORY_MANAGER_SIZE;
    }
    void * aux = currentAddress;
    currentAddress += size;
    return aux;
}

void initList(MemoryManagerADT mm) {
	mm->root			= (FreeList) allocFirst(STRUCTURE_SIZE);
	mm->dataMemory		= allocFirst(ALLOC_BLOCK);
	mm->root->data		= mm->dataMemory;
	mm->root->size		= ALLOC_BLOCK;
	mm->root->ocuppied	= false;
	mm->root->prev		= NULL;
	mm->root->next		= NULL;
	lastPhysicalAddress = mm->root;
}

MemoryManagerADT createMM() {
    MemoryManagerADT mm = (MemoryManagerADT) HEAP_STARTING_ADDRESS;
    mm->free = ALLOC_BLOCK;
    mm->occupied = 0;
    initList(mm); // FreeList init
}

void * allocMemory(MemoryManagerADT mm, size_t size) {
    if (size <= 0 || mm == NULL) {
        return NULL;
    }
    void * address = insert(mm,size);
    if (address != NULL) {
        mm->free -= size;
        mm->occupied += size;
    }
    return address;
}

int freeMemory(MemoryManager mm, void *data){
    size_t size = delete(mm, data);
    if (size == -1){
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
void * insert(MemoryManagerADT mm, size_t size) {
	FreeList current = mm->root;
	/* iterate until you find a block where (the data is null and the size
	 * needed fits) or when the next is null */
	while (!((current->ocuppied == false && current->size >= size) ||
			 current->next == NULL)) {
		current = current->next;
	}
	if (current->ocuppied == false) {
        if(current->size == size){
            current->occupied = true;
            return current->data;
        }
		current->ocuppied = true;
		FreeList emptyRemaining	 = lastPhysicalAddress + sizeof(Node);
        lastPhysicalAddress      = emptyRemaining;
        emptyRemaining->next     = current->next;
		current->next			 = emptyRemaining;
        emptyRemaining->prev	 = current;
		emptyRemaining->size	 = current->size - size;
		emptyRemaining->data	 = current->data + size;
		emptyRemaining->ocuppied = false;
	}
	else {
		return NULL;
	}
	return current->data;
}


/**
 * @brief 
 * 
 * @param data 
 * @return * int Devuelve 1 si libero correctamente la memoria y 0 si no encontro la memoria a liberar
 */
size_t delete(MemoryManager mm, void *data) {
    size_t toReturn = -1;
	FreeList current = mm->root;
	while (current->data != data && current->next != NULL) {
		current = current->next;
	}
	if (current->next == NULL) return toReturn;

	//liberar el espacio
    toReturn = current->size;
    
	//si tanto el anterior como el siguiente apuntan a espacio ocupado es solo marcar como desocupado
	if (current->prev->ocuppied && current->next->occupied){
		current->occupied = false;
		current->data = NULL;
	} 
    
    else if (!current->prev->occupied) {
		//unicamente el anterior apunta a espacio libre
		if(current->next->occupied){
			current->prev->size += current->size;
			current->prev->next = current->next;
			moveLastPhysicalAddress(current);

		}		
        //si tanto el anterior como el siguiente apuntaban a espacios libres
        else{
		    current->prev->size += current->size + current->next->size;
		    current->prev->next = current->next->next;
		    moveLastPhysicalAddress(current->next);
		    moveLastPhysicalAddress(current);
        }
	}
    
	//unicamente el siguiente apunta a espacio libre
	else {
		current->occupied = false;
		current->size += current->next->size;
		current->next = current->next->next;
		moveLastPhysicalAddress(current->next);
	}
    
    return toReturn;
}

void moveLastPhysicalAddress(FreeList node) {
	node->data = lastPhysicalAddress->data;
	node->size = lastPhysicalAddress->size;
	node->occupied = lastPhysicalAddress->occupied;
	node->prev = lastPhysicalAddress->prev;
	node->next = lastPhysicalAddress->next;

	node->prev->next = node;
	lastPhysicalAddress -= sizeof(Node); 
}