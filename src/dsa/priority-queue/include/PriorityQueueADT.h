#define _PRIORITY_QUEUE_ADT_H_

typedef struct PriorityQueueCDT * PriorityQueueADT;

// Tipo de la data que se desea incluir en cada nodo de la cola de prioridad
typedef int V;

// Estructura donde se declara la prioridad y la data que se desea utilizar para la PriorityQueue
typedef struct HeapNode {
    int priority;
    V data;
} HeapNode;

PriorityQueueADT createHeap(int capacity);
void queue(PriorityQueueADT heap, HeapNode data);
HeapNode dequeue(PriorityQueueADT heap);