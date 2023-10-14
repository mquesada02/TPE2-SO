#define _FREE_LIST_H

#include "../../include/MemoryManagerADT.h" // change path for purpose

typedef struct Node {
    void * data; // address of the alloc data
    size_t size;
    char ocuppied;
    FreeList prev;
    FreeList next;
} Node;


typedef Node * FreeList;


FreeList init(MemoryManagerADT mm);
FreeList insert(void * data, size_t size);
void delete(void * data);