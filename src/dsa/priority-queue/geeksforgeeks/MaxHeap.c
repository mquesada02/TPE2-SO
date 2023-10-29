#include <stdlib.h> // después reemplazar por nuestro allocMemory en vez de malloc

// tipo incluido dentro del HeapNode
typedef int V;

// tipo del array del Heap
typedef struct HeapNode {
    int priority;
    V data;
} HeapNode;

// Heap
typedef struct Heap {
    HeapNode *arr;
    int currentSize;
    int capacity;
} Heap;

Heap * createHeap(int capacity) {
    Heap * heap = (Heap *) malloc(sizeof(Heap));

    if (heap == NULL)
        return NULL;

    heap->currentSize = 0;
    heap->capacity = capacity;

    heap->arr = (HeapNode *) malloc(sizeof(HeapNode) * capacity);

    if (heap->arr == NULL)
        return NULL;

    return heap;
}

void insertHelper(Heap * heap, int index) {
    int parent = (index - 1) / 2;
    if (heap->arr[parent].priority < heap->arr[index].priority) {
        HeapNode tempNode = heap->arr[parent];
        heap->arr[parent] = heap->arr[index];
        heap->arr[index] = tempNode;
        insertHelper(heap,parent);
    }
}

void insert(Heap * heap, HeapNode data) {
    if (heap->currentSize < heap->capacity){
        heap->arr[heap->currentSize] = data;
        insertHelper(heap, heap->currentSize);
        heap->currentSize++;
    }
}

void maxHeapify(Heap * heap, int index) {
    int left = index * 2 + 1;
    int right = index * 2 + 2;
    int max = index;

    if (left >= heap->currentSize || left < 0) {
        left = -1;
    }
    if (right >= heap->currentSize || right < 0) {
        right = -1;
    }

    if (left != -1 && heap->arr[left].priority > heap->arr[max].priority)
        max = left;
    if (right != -1 && heap->arr[right].priority > heap->arr[max].priority)
        max = right;

    if (max != index) {
        HeapNode tempNode = heap->arr[max];
        heap->arr[max] = heap->arr[index];
        heap->arr[index] = tempNode;

        maxHeapify(heap,max);
    }
}

HeapNode deleteNode(Heap * heap) {
    
    if (heap->currentSize == 0) {
        return;
    }

    HeapNode deleteItem = heap->arr[0];
    heap->arr[0] = heap->arr[heap->currentSize - 1];

    heap->currentSize--;

    maxHeapify(heap, 0);

    return deleteItem;
}