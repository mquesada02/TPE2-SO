#include <MemoryManagerADT.h>
#include <stdint.h>
#include <library.h>
#include <syscalls.h>

void * allocMemory(size_t size) {
	return syscall_allocMemory(size);
}
int freeMemory(void *data) {
	return syscall_freeMemory(data);
}
void printMemStatus() {
	size_t free, occupied;
	syscall_getMemStatus(&free, &occupied);
	printf("Free: %d\nOccupied: %d\nTotal: %d\n", free,occupied, free + occupied);
}