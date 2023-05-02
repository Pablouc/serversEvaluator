#include "include/linkedList.h"
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// function to initialize a new linked list
void initList(struct linkedList *list) {
  list->head = NULL;
  list->size = 0;
  sem_init(&list->list_sem, 0, 1);
}

// function to insert a new node at the end of the list

void insertAtEnd(struct linkedList *list, char *image) {
  sem_wait(&list->list_sem);
  struct node *newNode;
  newNode = (struct node *)malloc(sizeof(struct node));
  strcpy(newNode->image, image);
  newNode->next = NULL;

  if (list->head == NULL) {
    list->head = newNode;
    list->tail = newNode;
  } else {
    list->tail->next = newNode;
    list->tail = newNode;
  }

  list->size++;
  sem_post(&list->list_sem);
}

// function to remove the first node from the list
struct node pop(struct linkedList *list) {
  sem_wait(&list->list_sem);
  struct node result = (struct node){};
  if (list->head != NULL) {
    result = *(list->head);
    struct node *oldHead = list->head;
    list->head = oldHead->next;
    if (oldHead == list->tail) {
      list->tail = NULL;
    }
    free(oldHead);
  }
  list->size--;
  sem_post(&list->list_sem);
  return result;
}

// function to print the contents of the linked list
void printList(struct linkedList *list) {
  struct node *current = list->head;
  while (current != NULL) {
    printf("%s ", current->image);
    current = current->next;
  }
  printf("\n");
}
