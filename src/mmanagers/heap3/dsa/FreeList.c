#include <FreeList.h>
#include <sys/types.h>

#define STRUCTURE_SIZE HEAP_END_ADDRESS-HEAP_STARTING_ADDRESS

FreeList root = NULL; // en root se guarda el starting address


FreeList init(MemoryManagerADT mm) {
    root = (FreeList) allocMemory(mm, STRUCTURE_SIZE);
    root->data = NULL;
    root->size = NULL;
    root->prev = NULL;
    root->next = NULL;
    return root;
}

FreeList insert(void * data, size_t size) {
    FreeList current = root;
    /* iterate until you find a block where (the data is null and the size needed fits) or when the next is null */
    while (!((current->data == NULL && current->size >= size) || current->next == NULL)) {
        current = current->next;
    }   
    if (current->data == NULL) {
        /* prev was already set */
        current->data = data;
    } else {
        /* need to set prev; we are at the end of the list */
        FreeList new = current + current->size + 1; // +1??
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


