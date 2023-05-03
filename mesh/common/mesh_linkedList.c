#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include "mesh_linkedList.h"

#include "../include/mesh.h"

// Define the linked list node structure

struct process *get_process_by_id(struct mesh_node* head, int id) {
    // Traverse the linked list to find the node with the matching ID
    struct mesh_node* current_node = head;
    while (current_node != NULL) {
        if (current_node->data->id == id) {
            return current_node->data;
        }
        current_node = current_node->next;
    }

    // If the node with the matching ID is not found, return a default process structure
    return NULL;
}

// Function to add a node to the end of the linked list
void add_node(struct mesh_node** head_ref, struct process *new_data) {
    // Allocate memory for the new node
    struct mesh_node* new_node = (struct mesh_node*)malloc(sizeof(struct mesh_node));
    // Assign data to the new node
    new_node->data = new_data;
    new_node->next = NULL;

    // If the linked list is empty, make the new node the head node
    if (*head_ref == NULL) {
        *head_ref = new_node;
        return;
    }

    // Traverse the linked list to find the last node
    struct mesh_node* last_node = *head_ref;
    while (last_node->next != NULL) {
        last_node = last_node->next;
    }

    // Append the new node to the end of the linked list
    last_node->next = new_node;
}

// Function to remove a node from the linked list by index
void remove_node(struct mesh_node** head_ref, int index) {
    // If the linked list is empty, return
    if (*head_ref == NULL) {
        return;
    }

    // If the node to be removed is the head node, update the head node
    if (index == 0) {
        struct mesh_node* temp = *head_ref;
        *head_ref = temp->next;
        free(temp);
        return;
    }

    // Traverse the linked list to find the node to be removed
    struct mesh_node* prev_node = *head_ref;
    struct mesh_node* curr_node = prev_node->next;
    int i = 1;
    while (curr_node != NULL && i != index) {
        prev_node = curr_node;
        curr_node = curr_node->next;
        i++;
    }

    // If the node to be removed is not found, return
    if (curr_node == NULL) {
        return;
    }

    // Remove the node from the linked list
    prev_node->next = curr_node->next;
    free(curr_node);
}

/* // Function to print the linked list
void print_list(struct node* head) {
    struct node* current_node = head;
    while (current_node != NULL) {
        printf("Image Number: %d\n", current_node->data.image_num);
        printf("Semaphore: %p\n", &current_node->data.go);
        printf("ID: %d\n", current_node->data.id);
        printf("Image: %s\n", current_node->data.image);
        printf("\n");
        current_node = current_node->next;
    }
} */