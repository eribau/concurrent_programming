/* Compile with:
    gcc -o hungrybirds hungrybirds.c -lpthread -lm
   Run with:
    ./cars <number of birds> <number of worms>
*/

#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <unistd.h>
#define SHARED 1
#define BABYBIRDS 3
#define WORMS 10

void *parentBird(void *);  /* the two threads */
void *babyBird(void *);

sem_t empty, nonempty;    /* the global semaphores */
int dish;                 /* shared dish of worms */
int w;                    /* number of worms */
int n;                    /* number of baby birds */

void rest() {
  int time = rand()%3;
  sleep(time);
}

/* read command line and create threads */

int main(int argc, char *argv[]) {
  int i;
  long id;

  /* read command line arguments, if any */
  n = (argc > 1) ? atoi(argv[1]) : BABYBIRDS;
  w = (argc > 2) ? atoi(argv[2]) : WORMS;

  /* thread ids and attributes */
  pthread_t pid;
  pthread_t bid[n];
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  sem_init(&empty, SHARED, 0);  /* sem empty = 0 */
  sem_init(&nonempty, SHARED, 1);   /* sem nonempty = 1  */

  dish = w;

  printf("simulation started\n");
  rest();
  pthread_create(&pid, &attr, parentBird, NULL);
  for (id = 1; id <= n; id++) {
    pthread_create(&bid[id], &attr, babyBird, (void *) id);
  }
}

/* The parent bird refills the dish if called by a baby bird */
void *parentBird(void *arg) {
  while (1) {
    sem_wait(&empty);
    dish = w;
    printf("The parent bird filled the dish\n");
    sem_post(&nonempty);
  }
}

/* A baby bird checks if there are worms to eat, if not
 it calls the parent bird */
void *babyBird(void *arg) {
  long id = (long) arg;
  printf("Baby bird %d hatched\n", id);
  while (1) {
    sem_wait(&nonempt y);
    if (dish <= 0) {
      printf("Baby bird %d found the dish to be empty\n", id);
      sem_post(&empty);
    } else {
      dish--;
      printf("Baby bird %d ate a worm\n", id);
      sem_post(&nonempty);
      rest();
    }
  }
}
