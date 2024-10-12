#include <stdio.h>
#include <stdlib.h>

// Global variables
int global1;
int global2;
int global3;

void recursive_function(int level) {
    if (level > 10) return;  // Base case: stop recursion after 3 levels

    int local = level * 10;  // Local variable
    int *heap = (int*)malloc(sizeof(int));  // Allocate memory on heap
    *heap = level * 100;

    printf("Level %d:\n", level);
    printf("  Address of local: %p\n", (void*)&local);
    printf("  Address of heap: %p\n", (void*)heap);
    printf("  Address of global1: %p\n", (void*)&global1);
    printf("  Address of global2: %p\n", (void*)&global2);
    printf("  Address of global3: %p\n", (void*)&global3);

    // Recursive call
    recursive_function(level + 1);

    // Free the allocated memory
    free(heap);
}

int main() {
    printf("Address of global1: %p\n", (void*)&global1);
    printf("Address of global2: %p\n", (void*)&global2);
    printf("Address of global3: %p\n", (void*)&global3);
    
    recursive_function(1);
    
    return 0;
}