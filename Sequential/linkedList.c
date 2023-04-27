#include <stdio.h>
#include <stdlib.h>

struct node {
    int data;
    struct node* next;
};


struct linkedList{
    struct node *head;
    struct node *current;
};


// function to initialize a new linked list
void initList(struct linkedList* list) {
    list->head = NULL;
}



// function to insert a new node at the end of the list
void insertAtEnd(struct linkedList* list, int data) {
    struct node* newNode = (struct node*) malloc(sizeof(struct node));
    newNode->data = data;
    newNode->next = NULL;
    if (list->head == NULL) {
        list->head = newNode;
    } else {
        struct node* current = list->head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

// function to insert a new node at the beginning of the list
void insertAtBeginning(struct linkedList* list, int data) {
    struct node* newNode = (struct node*) malloc(sizeof(struct node));
    newNode->data = data;
    newNode->next = list->head;
    list->head = newNode;
}

// function to remove the first node from the list
void removeFromBeginning(struct linkedList* list) {
    if (list->head != NULL) {
        struct node* oldHead = list->head;
        list->head = oldHead->next;
        free(oldHead);
    }
}


// function to print the contents of the linked list
void printList(struct linkedList* list) {
    struct node* current = list->head;
    while (current != NULL) {
        printf("%d ", current->data);
        current = current->next;
    }
    printf("\n");
}


