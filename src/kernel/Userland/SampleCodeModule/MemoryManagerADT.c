#include <MemoryManagerADT.h>
#include <stdint.h>
#include <library.h>

#define NULL (void*) 0

#define true 1
#define false 0

typedef struct Node * FreeList;

typedef struct Node {
    void * data; // address of the alloc data
    size_t size; // size of data in the alloc blocks
    char occupied;
    FreeList prev;
    FreeList next;
} Node;


extern void * syscall_init_list(MemoryManagerADT mm, FreeList* lastPhysicalAddress, size_t* alloc_block);

void * insert(size_t size);
size_t delete(void *data);
void moveLastPhysicalAddress(FreeList node);

FreeList lastPhysicalAddress = NULL;
size_t ALLOC_BLOCK;

typedef struct MemoryManagerCDT {
    uint64_t free;
    uint64_t occupied;
    FreeList root;
    void * dataMemory;
} MemoryManagerCDT;

static MemoryManagerADT mm = NULL;

void createMM() {
    mm = (MemoryManagerADT) 0x0000000000050000;
	syscall_init_list(mm, &lastPhysicalAddress, &ALLOC_BLOCK);
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
	printf("address %d\n",(size_t)address);
	printf("size %d\n",size);
    return address;
}

int freeMemory(void *data) {
	if (mm == NULL)
		return false;
    size_t size = delete(data);
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
void * insert(size_t size) {
	FreeList current = mm->root;
	/* iterate until you find a block where (the data is null and the size
	 * needed fits) or when the next is null */
	while (!((current->occupied == false && current->size >= size) ||
			 current->next == NULL)) {
		current = current->next;
	}
	if (current->occupied == false) {
        if(current->size == size){
            current->occupied = true;
            return current->data;
        }
		current->occupied = true;
		FreeList emptyRemaining	 = lastPhysicalAddress + sizeof(Node);
        lastPhysicalAddress      = emptyRemaining;
        emptyRemaining->next     = current->next;
		current->next			 = emptyRemaining;
        emptyRemaining->prev	 = current;
		emptyRemaining->size	 = current->size - size;
		emptyRemaining->data	 = current->data + size;
		emptyRemaining->occupied = false;
	}
	else {
		printf("next: %d\n",current->next);
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
size_t delete(void *data) {
    size_t toReturn = -1;
	FreeList current = mm->root;
	while (current->data != data && current->next != NULL) {
		current = current->next;
	}
	if (current->next == NULL) return toReturn;

	//liberar el espacio
    toReturn = current->size;
    
	//si tanto el anterior como el siguiente apuntan a espacio ocupado es solo marcar como desocupado
	if (current->prev->occupied && current->next->occupied){
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