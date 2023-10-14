#include <FreeList.h>
#include <sys/types.h>

#define STRUCTURE_SIZE (HEAP_END_ADDRESS - HEAP_STARTING_ADDRESS) / 4
#define ALLOC_BLOCK (HEAP_END_ADDRESS - HEAP_STARTING_ADDRESS) * 3 / 4

#define true 1
#define false 0

FreeList root	 = NULL; // en root se guarda el starting address
void *dataMemory = NULL;

void *lastPhysicalAddress = NULL;

FreeList init(MemoryManagerADT mm) {
	root				= (FreeList) allocFirst(mm, STRUCTURE_SIZE);
	dataMemory			= allocFirst(mm, ALLOC_BLOCK);
	root->data			= dataMemory;
	root->size			= ALLOC_BLOCK;
	root->ocuppied		= false;
	root->prev			= NULL;
	root->next			= NULL;
	lastPhysicalAddress = root;
	return root;
}

FreeList insert(void *data, size_t size) {
	FreeList current = root;
	/* iterate until you find a block where (the data is null and the size
	 * needed fits) or when the next is null */
	while (!((current->ocuppied == false && current->size >= size) ||
			 current->next == NULL)) {
		current = current->next;
	}
	if (current->ocuppied == false) {
		current->ocuppied = true;
		// *(current->data) = *data;
		if (current->next == NULL) { // estoy al final de toda la lista, necesito crear otro con el size del remaining. Sino, estoy parado en un bloque normal suelto y no hace falta crearlo porque ya existe (al final de la lista)
			FreeList emptyRemaining	 = current + sizeof(Node);
			current->next			 = emptyRemaining;
			emptyRemaining->size	 = current->size - size;
			emptyRemaining->data	 = current->data + size;
			emptyRemaining->ocuppied = false;
			emptyRemaining->prev	 = current;
			emptyRemaining->next	 = NULL;
			lastPhysicalAddress		 = emptyRemaining;
		}
	}
	else {
		// error. No space left
	}
	return current->data;
}

void delete(void *data) {
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
				current				= current->prev;
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
