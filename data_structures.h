#include <stdlib.h>

// Process states
typedef enum ProcessState
{
    READY,
    RUNNING,
    TERMINATED
} ProcessState;

// Process Control Block structure
typedef struct PCB
{
    int id;
    int pid;
    int priority;
    int running_time;
    int arrival_time;
    int start_time;
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
void init_PriorityQueue(PriorityQueue *queue)
{
    queue->head = NULL;
}

// Function to check if the priority queue is empty
int isEmpty_PriorityQueue(PriorityQueue *queue)
{
    return queue->head == NULL;
}

// Function to insert a process into the priority queue based on its priority
void enqueue_PriorityQueue(PriorityQueue *queue, PCB process, int priority)
{
    PQ_Node *newNode = (PQ_Node *)malloc(sizeof(PQ_Node));
    newNode->process = process;
    newNode->priority = priority;
    newNode->next = NULL;

    // If the queue is empty or the new node has higher priority, insert at the beginning
    if (isEmpty_PriorityQueue(queue) || priority < queue->head->priority)
    {
        newNode->next = queue->head;
        queue->head = newNode;
    }
    else
    {
        // Traverse the queue to find the correct position to insert
        PQ_Node *current = queue->head;
        while (current->next != NULL && current->next->priority <= priority)
        {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

// Function to remove and return the highest priority process from the priority queue
PCB dequeue_PriorityQueue(PriorityQueue *queue)
{
    PCB process = queue->head->process;
    queue->head = queue->head->next;
    return process;
}

// Function to destroy the priority queue and free memory
void destroy_PriorityQueue(PriorityQueue *queue)
{
    PQ_Node *current = queue->head;
    while (current != NULL)
    {
        PQ_Node *temp = current;
        current = current->next;
        free(temp);
    }
}

// -----Test-----
// PriorityQueue pq;
// init_PriorityQueue(&pq);
// PCB process1 = {1, 1562, 2, 0, 0, 0, 10, 0, READY};
// enqueue_PriorityQueue(&pq, process1, process1.priority);
// dequeue_PriorityQueue(&pq)

//--------------------------------------------------Circular Queue--------------------------------------------------

// Node structure for circular queue
typedef struct CQ_Node
{
    PCB process;
    struct CQ_Node *next;
} CQ_Node;

// Circular queue structure
typedef struct CircularQueue
{
    CQ_Node *front;
    CQ_Node *rear;
} CircularQueue;

// Function to initialize a circular queue
void init_CircularQueue(CircularQueue *queue)
{
    queue->front = queue->rear = NULL;
}

// Function to check if the circular queue is empty
int isEmpty_CircularQueue(CircularQueue *queue)
{
    return queue->front == NULL;
}

// Function to enqueue a node into the circular queue
void enqueue(CircularQueue *queue, PCB process)
{
    CQ_Node *newNode = (CQ_Node *)malloc(sizeof(CQ_Node));
    newNode->process = process;
    newNode->next = NULL;

    if (queue->front == NULL)
    {
        queue->front = queue->rear = newNode;
    }
    else
    {
        queue->rear->next = newNode;
        queue->rear = newNode;
    }

    queue->rear->next = queue->front;
}

// Function to delete a node with a specific value from the circular queue
void deleteNode_CircularQueue(CircularQueue *queue, int PID)
{
    if (queue->front == NULL)
    {
        return;
    }

    CQ_Node *current = queue->front;
    CQ_Node *prev = NULL;

    // Traverse the queue to find the node with the PID
    do
    {
        if (current->process.pid == PID)
        {
            if (current == queue->front)
            {
                // If the node to be deleted is the front node
                queue->front = current->next;
                queue->rear->next = queue->front;
                free(current);
                return;
            }
            else if (current == queue->rear)
            {
                // If the node to be deleted is the rear node
                prev->next = queue->front;
                queue->rear = prev;
                free(current);
                return;
            }
            else
            {
                // If the node to be deleted is in between
                prev->next = current->next;
                free(current);
                return;
            }
        }

        prev = current;
        current = current->next;

    } while (current != queue->front);
}

// Function to dequeue the front node from the circular queue
PCB dequeue_CircularQueue(CircularQueue *queue)
{
    CQ_Node *temp = queue->front;
    PCB process = temp->process;

    if (queue->front == queue->rear)
    {
        // If there is only one node in the queue
        queue->front = queue->rear = NULL;
    }
    else
    {
        queue->front = queue->front->next;
        queue->rear->next = queue->front;
    }

    free(temp);
    return process;
}

// Function to destroy the circular queue and free memory
void destroy_CircularQueue(CircularQueue *queue)
{
    CQ_Node *current = queue->front;
    while (current != NULL)
    {
        CQ_Node *temp = current;
        current = current->next;
        free(temp);
    }
}