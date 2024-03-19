/*
 * Tutorial 8 Signals and Data Structures Part II for SOFE 3950: Operating Systems
 *
 * Copyright (C) 2024, <Mostafa Abedi, Nathaniel Manoj, Calvin Reveredo>
 * All rights reserved.
 *
*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LEN 256

// Structure for process
struct proc {
    char parent[MAX_LEN];
    char name[MAX_LEN];
    int priority;
    int memory;
    struct proc *left;
    struct proc *right;
};

// Binary tree structure
struct proc_tree {
    struct proc *root;
};

// Function to create a new process node
struct proc *create_proc_node(char parent[], char name[], int priority, int memory) {
    struct proc *new_proc = (struct proc *)malloc(sizeof(struct proc));
    if (new_proc == NULL) {
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    strcpy(new_proc->parent, parent);
    strcpy(new_proc->name, name);
    new_proc->priority = priority;
    new_proc->memory = memory;
    new_proc->left = NULL;
    new_proc->right = NULL;
    return new_proc;
}

// Function to insert a process node into the binary tree
void insert_proc_node(struct proc **root, struct proc *node) {
    if (*root == NULL) {
        *root = node;
    } else {
        if (strcmp(node->parent, (*root)->name) < 0)
            insert_proc_node(&(*root)->left, node);
        else
            insert_proc_node(&(*root)->right, node);
    }
}

// Function to read data from file and construct the binary tree
void construct_proc_tree(struct proc_tree *tree, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file\n");
        exit(EXIT_FAILURE);
    }

    char parent[MAX_LEN], name[MAX_LEN];
    int priority, memory;

    while (fscanf(file, "%[^,],%[^,],%d,%d\n", parent, name, &priority, &memory) != EOF) {
        struct proc *new_proc = create_proc_node(parent, name, priority, memory);
        insert_proc_node(&(tree->root), new_proc);
    }

    fclose(file);
}

// Function to print the contents of the binary tree using inorder traversal
void print_proc_tree(struct proc *root) {
    if (root != NULL) {
        print_proc_tree(root->left);
        printf("Parent: %s, Name: %s, Priority: %d, Memory: %d MB\n", root->parent, root->name, root->priority, root->memory);
        print_proc_tree(root->right);
    }
}

int main() {
    struct proc_tree tree = {NULL};
    construct_proc_tree(&tree, "processes_tree.txt");
    printf("Contents of the process tree:\n");
    print_proc_tree(tree.root);

    return 0;
}