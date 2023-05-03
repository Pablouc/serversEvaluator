#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include <semaphore.h>

sem_t *list_sem;

struct linkedList {
  struct node *head;
  struct node *current;
  struct node *tail;
  int size;
  int image_num;
  sem_t list_sem;
};

struct node {
  char image[50];
  struct node *next;
  int image_num;
};

void initList(struct linkedList *list);
void insertAtEnd(struct linkedList *list, char *image);
struct node pop(struct linkedList *list);
void printList(struct linkedList *list);
int get_image_num(struct linkedList *list);

#endif
