#include <stdlib.h>

// Process states
typedef enum
{
    READY,
    RUNNING,
    BLOCKED,
    TERMINATED
} ProcessState;

// Process Control Block structure
typedef struct PCB
{
    int id;
    int pid;
    int priority;
    int arrival_time;
    int running_time;
    int waiting_time;
    int remaining_time;
    int finish_time;
    ProcessState state;
} PCB;

//--------------------------------------------------Priority Queue--------------------------------------------------

// Node structure for the priority queue
typedef struct PQ_Node
{
    PCB process;
    int priority;
    struct PQ_Node *next;
} PQ_Node;

// Priority Queue structure
typedef struct PriorityQueue
{
    PQ_Node *head;
} PriorityQueue;

// Function to initialize a priority queue
void initPriorityQueue(PriorityQueue *pq)
{
    pq->head = NULL;
}

// Function to check if the priority queue is empty
int isEmpty(PriorityQueue *pq)
{
    return pq->head == NULL;
}

// Function to insert a process into the priority queue based on its priority
void enqueue(PriorityQueue *pq, PCB process, int priority)
{
    PQ_Node *newNode = (PQ_Node *)malloc(sizeof(PQ_Node));
    newNode->process = process;
    newNode->priority = priority;
    newNode->next = NULL;

    // If the queue is empty or the new node has higher priority, insert at the beginning
    if (isEmpty(pq) || priority < pq->head->priority)
    {
        newNode->next = pq->head;
        pq->head = newNode;
    }
    else
    {
        // Traverse the queue to find the correct position to insert
        PQ_Node *current = pq->head;
        while (current->next != NULL && current->next->priority <= priority)
        {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

// Function to remove and return the highest priority process from the priority queue
PCB dequeue(PriorityQueue *pq)
{
    PCB process = pq->head->process;
    pq->head = pq->head->next;
    return process;
}

// Function to destroy the priority queue and free memory
void destroyPriorityQueue(PriorityQueue *pq)
{
    PQ_Node *current = pq->head;
    while (current != NULL)
    {
        PQ_Node *temp = current;
        current = current->next;
        free(temp);
    }
}

// -----Test-----
// PriorityQueue pq;
// initPriorityQueue(&pq);
// PCB process1 = {1, 1562, 2, 0, 0, 0, 10, 0, READY};
// enqueue(&pq, process1, process1.priority);
// dequeue(&pq)
