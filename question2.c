/*
 * Tutorial 8 Signals and Data Structures Part II for SOFE 3950: Operating Systems
 *
 * Copyright (C) 2024, <Mostafa Abedi, Nathaniel Manoj, Calvin Reveredo>
 * All rights reserved.
 *
*/
#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_LEN 256
#define MEMORY 1024

// Structure for process
struct proc {
    char name[MAX_LEN];
    int priority;
    int pid;
    int address;
    int memory;
    int runtime;
    int suspended;
};

// FIFO Queue structure
struct Queue {
    struct proc *array[MAX_LEN];
    int front, rear, size;
    unsigned capacity;
};

// Function to create a new process node
struct proc *create_proc_node(char name[], int priority, int memory, int runtime) {
    struct proc *new_proc = (struct proc *)malloc(sizeof(struct proc));
    if (new_proc == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    strcpy(new_proc->name, name);
    new_proc->priority = priority;
    new_proc->pid = 0;
    new_proc->address = 0;
    new_proc->memory = memory;
    new_proc->runtime = runtime;
    new_proc->suspended = 0;
    return new_proc;
}

// Function to create a new queue
struct Queue *createQueue(unsigned capacity) {
    struct Queue *queue = (struct Queue *)malloc(sizeof(struct Queue));
    if (queue == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    return queue;
}

// Function to check if queue is full
int isFull(struct Queue *queue) {
    return (queue->size == (int)queue->capacity);
}

// Function to check if queue is empty
int isEmpty(struct Queue *queue) {
    return (queue->size == 0);
}

// Function to add item to queue
void push(struct Queue *queue, struct proc *item) {
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1) % (int)queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
}

// Function to remove item from queue
struct proc *pop(struct Queue *queue) {
    if (isEmpty(queue))
        return NULL;
    struct proc *item = queue->array[queue->front];
    queue->front = (queue->front + 1) % (int)queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to execute the "process" binary
void execute_process(struct proc *process, int *avail_mem, struct Queue *secondary_queue) {
    printf("Executing process: Name: %s, Priority: %d, PID: %d, Memory: %d, Runtime: %d\n",
           process->name, process->priority, process->pid, process->memory, process->runtime);

    // Fork and exec the "process" binary
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        char memory_arg[10];
        sprintf(memory_arg, "%d", process->memory);
        execl("./process", "process", memory_arg, NULL);
        perror("exec failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process
        process->pid = pid;
        // Mark memory as used
        for (int i = process->address; i < process->address + process->memory; i++) {
            avail_mem[i] = 1;
        }
        // Run the process for the specified runtime or 1 second
        int run_time = (process->priority == 0) ? process->runtime : 1;
        sleep(run_time);
        // Send SIGTSTP to terminate the process
        kill(pid, SIGTSTP);
        waitpid(pid, NULL, WUNTRACED); // Wait for child to be stopped
        // Free memory used by process
        for (int i = process->address; i < process->address + process->memory; i++) {
            avail_mem[i] = 0;
        }
        // Decrement runtime and handle termination for secondary queue processes
        if (process->priority != 0 && process->runtime > 1) {
            process->runtime -= 1;
            // Set suspended flag and add back to the secondary queue
            process->suspended = 1;
            push(secondary_queue, process);
        } else {
            printf("Process %s terminated\n", process->name);
            free(process);
        }
    } else {
        // Fork failed
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
}

int main() {
    // Initialize queues
    struct Queue *priority_queue = createQueue(MAX_LEN);
    struct Queue *secondary_queue = createQueue(MAX_LEN);

    // Initialize available memory
    int avail_mem[MEMORY] = {0};

    // Read processes from file and populate queues
    FILE *file = fopen("processes_q2.txt", "r");
    if (file == NULL) {
        printf("Error opening file\n");
        exit(EXIT_FAILURE);
    }

    char name[MAX_LEN];
    int priority, memory, runtime;
    while (fscanf(file, "%[^,],%d,%d,%d\n", name, &priority, &memory, &runtime) != EOF) {
        struct proc *new_proc = create_proc_node(name, priority, memory, runtime);
        if (priority == 0)
            push(priority_queue, new_proc);
        else
            push(secondary_queue, new_proc);
    }
    fclose(file);

    // Execute processes in priority queue
    while (!isEmpty(priority_queue)) {
        struct proc *process = pop(priority_queue);
        execute_process(process, avail_mem, secondary_queue);
    }

    // Execute processes in secondary queue
    while (!isEmpty(secondary_queue)) {
        struct proc *process = pop(secondary_queue);
        execute_process(process, avail_mem, secondary_queue);
    }

    return 0;
}
