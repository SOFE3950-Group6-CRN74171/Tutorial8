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

// Function to execute a process
void execute_process(struct proc *process, int *avail_mem, struct Queue *secondary_queue) {
    printf("Executing process: Name: %s, Priority: %d, PID: %d, Memory: %d, Runtime: %d\n",
           process->name, process->priority, process->pid, process->memory, process->runtime);

    // Fork and exec
    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        char memory_arg[10];
        sprintf(memory_arg, "%d", process->memory);
        execl(process->name, process->name, memory_arg, NULL);
        perror("exec failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // Parent process
        process->pid = pid;
        // Mark memory as used
        for (int i = process->address; i < process->address + process->memory; i++) {
            avail_mem[i] = 1;
        }
        // Sleep for runtime seconds
        sleep(process->runtime);
        // Send SIGTSTP to terminate the process
        kill(pid, SIGTSTP);
        waitpid(pid, NULL, 0);
        // Free memory used by process
        for (int i = process->address; i < process->address + process->memory; i++) {
            avail_mem[i] = 0;
        }
        // Add the process back to the secondary queue
        push(secondary_queue, process);
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
    FILE *file = fopen("process", "r");
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
        // Check if there's enough memory available
        int enough_memory = 1;
        for (int i = 0; i < MEMORY; i++) {
            if (avail_mem[i] == 1) {
                enough_memory = 0;
                break;
            }
        }
        if (!enough_memory) {
            push(secondary_queue, process);
            continue;
        }
        execute_process(process, avail_mem, secondary_queue);
    }

    return 0;
}
