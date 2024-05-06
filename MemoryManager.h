#include <stdio.h>
#include <stdbool.h>

#define TOTAL_MEMORY_SIZE 1024
#define SMALLEST_BLOCK_SIZE 8

// Struct representing a memory block
typedef struct Block {
    int size;
    bool free;
    struct Block* next;
} Block;

// Global pointer to the start of the memory block
Block* memory = NULL;

// Function to initialize the memory
void initialize_memory() {
    memory = (Block*)malloc(TOTAL_MEMORY_SIZE);
    memory->size = TOTAL_MEMORY_SIZE;
    memory->free = true;
    memory->next = NULL;
}

// Function to allocate memory using the buddy system
void* allocate_memory(int size) {
    // Check if the memory is initialized
    if (memory == NULL) {
        initialize_memory();
    }
    
    // Find the smallest block size that can accommodate the requested size
    int blockSize = SMALLEST_BLOCK_SIZE;
    while (blockSize < size) {
        blockSize *= 2;
    }

    // Traverse the memory blocks to find a free block of the appropriate size
    Block* curr = memory;
    while (curr != NULL) {
        if (curr->free && curr->size >= blockSize) {
            // Split the block if necessary
            while (curr->size > blockSize * 2) {
                Block* buddy = (Block*)((char*)curr + curr->size / 2);
                buddy->size = curr->size / 2;
                buddy->free = true;
                buddy->next = curr->next;
                curr->next = buddy;
                curr->size /= 2;
            }
            // Mark the block as allocated
            curr->free = false;
            return (void*)curr;
        }
        curr = curr->next;
    }

    // No free block of appropriate size found
    return NULL;
}

// Function to deallocate memory
void deallocate_memory(void* ptr) {
    if (ptr == NULL) {
        return;
    }
    // Mark the block as free
    ((Block*)ptr)->free = true;
}

// Function to print memory status
void print_memory() {
    Block* curr = memory;
    while (curr != NULL) {
        printf("Block: %p, Size: %d, Free: %s\n", curr, curr->size, curr->free ? "Yes" : "No");
        curr = curr->next;
    }
}

int main() {
    // Allocate memory blocks
    void* ptr1 = allocate_memory(32);
    void* ptr2 = allocate_memory(64);
    void* ptr3 = allocate_memory(16);

    printf("Initial Memory Status:\n");
    print_memory();

    // Deallocate memory blocks
    deallocate_memory(ptr1);
    deallocate_memory(ptr2);

    printf("\nMemory Status After Deallocation:\n");
    print_memory();

    return 0;
}