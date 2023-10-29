#define MAX_SIZE 100
// Tipo de dato utilizado
typedef int V;

typedef struct {
    int priority;
    V data;
} HeapNode;

int parent(int i) {
    return (i-1)/2;
}

int leftChild(int i) {
    return 2 * i + 1;
}

int rightChild(int i) {
    return 2 * i + 2;
}

void swap(HeapNode * x, HeapNode * y) {
    V dataTemp = x->data;
    x->data = y->data;
    y->data = dataTemp;

    int priorityTemp = x->priority;
    x->priority = y->priority;
    y->priority = priorityTemp;
}

void enqueue(HeapNode pqueue[], HeapNode node, int * priority) {
    if (*priority >= MAX_SIZE) {
        // puede escribir un mensaje de error
        return;
    }
    // insertamos al final y después swappeamos
    pqueue[*priority] = node;
    *priority = *priority + 1;

    int i = *priority - 1;
    while (i != 0 && pqueue[parent(i)].priority < pqueue[i].priority) {
        swap(&pqueue[i], &pqueue[parent(i)]);
        i = parent(i);
    }
}

void maxHeapify(HeapNode pqueue[], int i, int priority) {
    int left = leftChild(i);
    int right = rightChild(i);
    int largest = i;

    if (left <= priority && pqueue[left].priority > pqueue[largest].priority) {
        largest = left;
    }

    if (right <= priority && pqueue[right].priority > pqueue[largest].priority) {
        largest = right;
    }

    if (largest != i) {
        HeapNode temp = pqueue[i];
        pqueue[i] = pqueue[largest];
        pqueue[largest] = temp;
        maxHeapify(pqueue,largest,priority);
    }
}

HeapNode getMax(HeapNode pqueue[]) {
    return pqueue[0];
}

HeapNode dequeue(HeapNode pqueue[], int * priority) {
    HeapNode max = getMax(pqueue);

    pqueue[0] = pqueue[*priority - 1];
    * priority = *priority - 1;

    maxHeapify(pqueue,0,*priority);
    return max;
}