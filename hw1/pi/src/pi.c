/* Calculate approximation of pi using pthreads
   usage under Linux:
     gcc matrixSum.c -lpthread
     a.out size numWorkers
*/
#ifndef _REENTRANT
#define _REENTRANT
#endif
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include "taskQueue.h"
#define DEF_EPSILON 0.1
#define MAXWORKERS 10
#define DEF_HEURISTIC 2

pthread_mutex_t lock; /* mutex lock for the bag of tasks */
pthread_mutex_t area;
pthread_mutex_t finished;
pthread_mutex_t ntasks;
int numWorkers;
double epsilon;
int numTasks;   /* number of generated tasks */
int finishedTasks;
int heuristic;  /* multiplier used to calculate maxTasks */
int maxTasks;
double sum;  /* The sum of the areas */


/* timer */
double read_timer() {
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }
    gettimeofday( &end, NULL );
    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

double start_time, end_time; /* start and end times */

void *Worker(void *);

/* Function that recursively calculates pi */
double pi(double a,double b,double fa,double fb,double area) {
  double m = (a + b)/2;
  double fm = sqrt(1 - m*m);
  double larea = (fa + fm)*(m - a)/2;
  double rarea = (fb + fm)*(b - m)/2;
  //printf("Area: %f Epsilon: %f\n", fabs((larea + rarea) - area), epsilon);
  if (fabs((larea + rarea) - area) > epsilon) {
    larea = pi(a, m, fa, fm, larea);
    rarea = pi(m, b, fm, fb, rarea);
  }
  return larea + rarea;
}

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  long l;
  pthread_attr_t attr;
  pthread_t workerid[MAXWORKERS];

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* initialize mutex */
  pthread_mutex_init(&lock, NULL);

  /* read command line arguments, if any */
  numWorkers = (argc > 1) ? atoi(argv[1]) : MAXWORKERS;
  epsilon = (argc > 2) ? atof(argv[2]) : DEF_EPSILON;
  heuristic = (argc > 3) ? atoi(argv[3]) : DEF_HEURISTIC;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  /* initialize the bag of tasks and variables */
  init();
  numTasks = 0;
  maxTasks = numWorkers * heuristic;
  finishedTasks = 0;
  sum = 0;

  /* Start the timer and do the work */
  start_time = read_timer();
  struct Task first = { 0.0, 1.0, 1.0, 0.0, 0.5};
  put(first);
  numTasks++;
  for(l = 0; l < numWorkers; l++) {
    pthread_create(&workerid[l], &attr, Worker, (void *) l);
  }
  for(l = 0; l < numWorkers; l++) {
    pthread_join(workerid[l], NULL);
  }
  end_time = read_timer();

  printf("Finished exectution in %f sec\n", end_time);
  printf("The calculated approximation of pi is %f\n", sum*4);

  pthread_exit(NULL);
}

/* Each worker sums the values in one strip of the matrix.
   After a barrier, worker(0) computes and prints the total */
void *Worker(void *arg) {
  double m, fm, larea, rarea;
  struct Task task;

  while(true) {
    if(numTasks < maxTasks) {
      pthread_mutex_lock(&ntasks);
      if(numTasks < maxTasks) {
        pthread_mutex_unlock(&ntasks);
        if(!isEmpty()) {
          pthread_mutex_lock(&lock);
          if(!isEmpty()) {
            task = get();
            pthread_mutex_unlock(&lock);

            m = (task.a + task.b)/2;
            fm = sqrt(1 - m*m);
            larea = (task.fa + fm)*(m - task.a)/2;
            rarea = (task.fb + fm)*(task.b - m)/2;
            if (fabs((larea + rarea) - task.area) > epsilon) {
                struct Task ltask = {task.a, m, task.fa, fm, larea};
                struct Task rtask = {m, task.b, fm, task.fb, rarea};
                pthread_mutex_lock(&lock);
                put(ltask);
                put(rtask);
                pthread_mutex_unlock(&lock);
                pthread_mutex_lock(&ntasks);
                numTasks = numTasks + 2;
                pthread_mutex_unlock(&ntasks);
            } else {
              pthread_mutex_lock(&area);
              sum = sum + larea + rarea;
              pthread_mutex_unlock(&area);
              pthread_mutex_lock(&finished);
              finishedTasks++;
              pthread_mutex_unlock(&finished);
            }
          } else {  // if the bag of tasks is empty
            pthread_mutex_unlock(&lock);
          }
        }
      } else {  // if numtasks >= maxTasks
          pthread_mutex_unlock(&ntasks);
          break;  // exit while-loop, since we have enough tasks
      }
    } else {
      break;
    }
  }
  while(true) {
    if(!isEmpty()) {
      pthread_mutex_lock(&lock);
      if(!isEmpty()) {
        task = get();
        pthread_mutex_unlock(&lock);

        double appr = pi(task.a, task.b, task.fa, task.fb, task.area);
        pthread_mutex_lock(&area);
        sum += appr;
        pthread_mutex_unlock(&area);
        pthread_mutex_lock(&finished);
        finishedTasks++;
        pthread_mutex_unlock(&finished);
      } else {  // the bag of tasks is empty
        pthread_mutex_unlock(&lock);
        break;  // nothing left to do
      }
    } else {
      break;
    }
  }
}
