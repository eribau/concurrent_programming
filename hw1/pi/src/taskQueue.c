#include <stdlib.h>
#include <stdio.h>

struct Task {
  double a;
  double b;
  double fa;
  double fb;
  double area;
};

int size;

typedef struct queueNode* link;
struct queueNode { struct Task task; link next; };
static link head, tail;

link new(struct Task task, link next) {
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

void put(struct Task task) {
  size++;
  if (head == NULL) {
    head = (tail = new(task, head));
    struct Task *ta = &tail->task;
    printf("Test2 %f\n", ta->a);
    return;
  }
  tail->next = new(task, tail->next);
  tail = tail->next;
}

 struct Task get() {
  struct Task task = head->task;
  link t = head->next;
  free(head);
  head = t;
  size--;
  struct Task *ta = &task;
  printf("Get %f\n", ta->a);
  return task;
}
