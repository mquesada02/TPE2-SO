#define _FREE_LIST_H

#include "../../include/MemoryManagerADT.h" // change path for purpose

typedef struct Node {
    void * data; // if data == null then this block is free
    size_t size;
    FreeList prev;
    FreeList next;
} Node;

typedef Node * FreeList;


FreeList init(MemoryManagerADT mm);
FreeList insert(void * data, size_t size);
void delete(void * data);