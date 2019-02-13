#include <stdlib.h>
#include <stdio.h>

struct IndexPair {
  int lo;
  int hi;
};

int size;

typedef struct queueNode* link;
struct queueNode { struct IndexPair task; link next; };
static link head, tail;

link new(struct IndexPair task, link next) {
  link x = malloc(sizeof(*x));
  x->task = task;
  x->next = next;
}

void init() {
  head = NULL;
  size = 0;
}

int isEmpty() {
  return size == 0;
}

void put(struct IndexPair task) {
  size++;
  if (head == NULL) {
    head = (tail = new(task, head));
    //printf("Put\n");
    return;
  }
  tail->next = new(task, tail->next);
  tail = tail->next;
}

 struct IndexPair get() {
  struct IndexPair task = head->task;
  link t = head->next;
  free(head);
  head = t;
  size--;
  //printf("Get\n");
  return task;
}
