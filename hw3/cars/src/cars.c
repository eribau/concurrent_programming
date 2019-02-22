/* Compile with:
    gcc -o cars cars.c -lpthread
   Run with:
    ./cars <number of trips> <number of cars>
*/

#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define SHARED 1
#define TRIPS 2
#define CARS 10
#define NORTHBOUND 0
#define SOUTHBOUND 1

void *car(void *);

sem_t entry, south, north;  /* the global semaphores */
int numS, numN;             /* the number of cars on the bridge */
int waitS, waitN;           /* the current number of cars waiting */
int t;                      /* the number of trips each cars makes */
int numCars;                /* the number of cars */

void turnAround() {
  sleep(2);
}

void crossBridge() {
  sleep(1);
}

/* read command line and create threads and wait for them to complete */

int main(int argc, char *argv[]) {
  int i;
  long id;

  /* read command line arguments, if any */
  t = (argc > 1) ? atoi(argv[1]) : TRIPS;
  numCars = (argc > 2) ? atoi(argv[2]) : CARS;

  /* thread ids and attributes */
  pthread_t cid[numCars];
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* initialize semaphores and variables */
  sem_init(&entry, SHARED, 1);
  sem_init(&south, SHARED, 0);
  sem_init(&north, SHARED, 0);
  numN = numS = 0;
  waitN = waitS = 0;

  printf("simulation started\n");
  for (id = 1; id <= numCars; id++) {
    pthread_create(&cid[id], &attr, car, (void *) id);
  }

  for (i = 0; i < numCars; i++) {
    pthread_join(cid[i], NULL);
  }
  printf("simulation over\n");
}

void *car(void *arg) {
  int direction;
  int trips = t;
  long id = (long) arg;
  if (id%2 == 0) {
    direction = SOUTHBOUND;
  } else {
    direction = NORTHBOUND;
  }
  printf("Car %d started travelling\n", id);

  while (trips > 0) {
    if (direction == SOUTHBOUND) {
      sem_wait(&entry);
      if (numN > 0) {
        waitS++;
        sem_post(&entry);
        sem_wait(&south);
      }
      numS++;
      if (waitS > 0) {
        waitS--;
        sem_post(&south);
      } else {
        sem_post(&entry);
      }
      /* Critical section */
      printf("Car %d is crossing the bridge, going south\n", id);
      crossBridge();
      direction = NORTHBOUND;
      sem_wait(&entry);
      numS--;
      /* ---------------- */
      if (numS == 0 && waitN > 0) {
        waitN--;
        sem_post(&north);
      } else {
        sem_post(&entry);
      }
    } else {  // if NORTHBOUND
      sem_wait(&entry);
      if (numS > 0) {
        waitN++;
        sem_post(&entry);
        sem_wait(&north);
      }
      numN++;
      if (waitN > 0) {
        waitN--;
        sem_post(&north);
      } else {
        sem_post(&entry);
      }
      /* Critical section */
      printf("Car %d is crossing the bridge, going north\n", id);
      crossBridge();
      direction = SOUTHBOUND;
      sem_wait(&entry);
      numN--;
      /* ---------------- */
      if (numN == 0 && waitS > 0) {
        waitS--;
        sem_post(&south);
      } else {
        sem_post(&entry);
      }
    }
    trips--;
    turnAround();
  }
}
