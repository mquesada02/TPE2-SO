#include <stdio.h>
#include <string.h>
#include "../include/SimpleMemoryManagerADT.h"

void testAllocMemory();
void testWriteMemory();
static MemoryManagerADT createMemoryManagerTest();
static void allocateMemoryTest(MemoryManagerADT mm, size_t size);
static void writeMemoryTest(MemoryManagerADT mm);

int main() {
    testAllocMemory();
    testWriteMemory();
    printf("\nAll tests passed.\n");
    return 0;
}


void testAllocMemory(){
    printf("\nTesting allocation of memory...\n\n");
    MemoryManagerADT mm = createMemoryManagerTest();
    printf("Successfully created memory manager\n");

    printf("Testing allocation of 1000 bytes of memory...\n");
    allocateMemoryTest(mm, 1000);
    printf("Successfully allocated 1000 bytes of memory\n");
    printf("Allocating variable bytes of memory...\n");
    for (int i=0; i<100; i++) {
        allocMemory(mm,i);
    }
    printf("\nSuccessfull allocation test.\n\n");
}

void testWriteMemory() {
    printf("\nTesting writing to memory...\n\n");
    printf("Creating memory manager...\n");
    MemoryManagerADT mm = createMemoryManagerTest();

    printf("Successfully created memory manager\n");

    printf("Testing writing to memory...\n");
    writeMemoryTest(mm);

    printf("\nSuccessfull memory write test\n\n");
}

static MemoryManagerADT createMemoryManagerTest() {
    MemoryManagerADT mm = createMM();
    if (mm == NULL) {
        printf("Memory Manager cannot be null");
    }
    return mm;
}

static void allocateMemoryTest(MemoryManagerADT mm, size_t size) {
    void * address = allocMemory(mm, size);
    if (address == NULL) {
        printf("Failed to allocate memory");
    }
    return;
}

static void writeMemoryTest(MemoryManagerADT mm) {
    char * string = "Hello World";
    size_t len = strlen(string)+1;
    void * address = allocMemory(mm, len);   
    for (int i=0;string[i];i++) {
        ((char *)address)[i] = string[i];
    }
    ((char *)address)[len-1] = '\0';
}
