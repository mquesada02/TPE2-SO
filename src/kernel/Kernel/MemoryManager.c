#include <MemoryManager.h>
#include <stdint.h>


typedef struct Node {
    void * data; // address of the alloc data
    size_t size; // size of data in the alloc blocks
    char occupied;
    FreeList prev;
    FreeList next;
} Node;

typedef struct MemoryManagerCDT {
    uint64_t free;
    uint64_t occupied;
    FreeList root;
    void * dataMemory;
} MemoryManagerCDT;

void * currentAddress = NULL;

void * allocFirst(size_t size) {
    if (currentAddress == NULL) {
        currentAddress = ((void *)(HEAP_STARTING_ADDRESS + MEMORY_MANAGER_SIZE));
    }
    void * aux = currentAddress;
    currentAddress += size;
    return aux;
}

void* initList(MemoryManagerADT mm, FreeList* lastPhysicalAddress, size_t* alloc_block) {
    
	mm->root			= (FreeList) allocFirst(STRUCTURE_SIZE);
	mm->dataMemory		= allocFirst(ALLOC_BLOCK);
	mm->root->data		= mm->dataMemory;
	mm->root->size		= ALLOC_BLOCK;
	mm->root->occupied	= false;
	mm->root->prev		= NULL;
	mm->root->next		= NULL;
	*lastPhysicalAddress = mm->root;
    *alloc_block = ALLOC_BLOCK;
    return HEAP_STARTING_ADDRESS;
}