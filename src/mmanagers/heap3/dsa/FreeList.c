#include <FreeList.h>
#include <sys/types.h>

#define STRUCTURE_SIZE (HEAP_END_ADDRESS-HEAP_STARTING_ADDRESS)/4
#define ALLOC_BLOCK (HEAP_END_ADDRESS-HEAP_STARTING_ADDRESS)*3/4


#define true 1
#define false 0

FreeList root = NULL; // en root se guarda el starting address
void * dataMemory = NULL;


void * lastPhysicalAddress = NULL;

FreeList init(MemoryManagerADT mm) {
    root = (FreeList) allocFirst(mm, STRUCTURE_SIZE);
    dataMemory = allocFirst(mm, ALLOC_BLOCK);
    root->data = ALLOC_BLOCK;
    root->size = NULL;
    root->ocuppied = 0;
    root->prev = NULL;
    root->next = NULL;
    lastPhysicalAddress = root;
    return root;
}

FreeList insert(void * data, size_t size) {
    FreeList current = root;
    /* iterate until you find a block where (the data is null and the size needed fits) or when the next is null */
    while (!((current->ocuppied == false && current->size >= size) || current->next == NULL)) {
        current = current->next;
    }   
    if (current->ocuppied == false) {
        /* prev was already set */
        // *(current->data) = *data;
        /* asigna */
        current->ocuppied = true;
    } else {
        /* need to set prev; we are at the end of the list */
        FreeList new = current + current->size + sizeof(Node) - sizeof(void*); // +1??
        new->data = data;
        new->size = size;
        new->prev = current;
        new->next = NULL;
    }

}

void delete(void * data) {
    FreeList current = root;
    while (current->data != data && current->next != NULL) {
        current = current->next;
    }
    if (current->data == data) {
        if (current->prev != NULL) {
            /* union adyacent previous blocks */
            if (current->prev->data == NULL) {
                current->prev->size += current->size;
                current->prev->next = current->next;
                current = current->prev;
            }
            current->data = NULL;
        }
        if (current->next != NULL) {
            if (current->next != NULL) {
                current->size += current->next->size;
                current->next = current->next->next;
            }
        }
    }
}


