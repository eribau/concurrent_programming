/* matrix summation using pthreads

   features: uses a barrier; the Worker[0] computes
             the total sum from partial sums computed by Workers
             and prints the total sum to the standard output

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
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 10   /* maximum number of workers */

pthread_mutex_t barrier;  /* mutex lock for the barrier */
pthread_cond_t go;        /* condition variable for leaving */
int numWorkers;           /* number of workers */
int numArrived = 0;       /* number who have arrived */

/* a reusable counter barrier */
void Barrier() {
  pthread_mutex_lock(&barrier);
  numArrived++;
  if (numArrived == numWorkers) {
    numArrived = 0;
    pthread_cond_broadcast(&go);
  } else
    pthread_cond_wait(&go, &barrier);
  pthread_mutex_unlock(&barrier);
}

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
int size, stripSize;  /* assume size is multiple of numWorkers */
int sums[MAXWORKERS]; /* partial sums */
int maxes[MAXWORKERS]; /* local maxes */
int mins[MAXWORKERS]; /* local mins */
int maxesPos[MAXWORKERS][2]; /* local max positions */
int minsPos[MAXWORKERS][2]; /* local min positions */
int minPos[2]; /* position in matrix of min */
int maxPos[2]; /* position in matrix of max */
int matrix[MAXSIZE][MAXSIZE]; /* matrix */

void *Worker(void *);

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j;
  long l; /* use long in case of a 64-bit system */
  pthread_attr_t attr;
  pthread_t workerid[MAXWORKERS];

  /* set global thread attributes */
  pthread_attr_init(&attr);
  pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

  /* initialize mutex and condition variable */
  pthread_mutex_init(&barrier, NULL);
  pthread_cond_init(&go, NULL);

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;
  stripSize = size/numWorkers;

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
	  for (j = 0; j < size; j++) {
          matrix[i][j] = rand()%99;
	  }
  }

  /* print the matrix */
#ifdef DEBUG
  for (i = 0; i < size; i++) {
	  printf("[ ");
	  for (j = 0; j < size; j++) {
	    printf(" %d", matrix[i][j]);
	  }
	  printf(" ]\n");
  }
#endif

  /* do the parallel work: create the workers */
  start_time = read_timer();
  for (l = 0; l < numWorkers; l++)
    pthread_create(&workerid[l], &attr, Worker, (void *) l);
  pthread_exit(NULL);
}

/* Each worker sums the values in one strip of the matrix.
   After a barrier, worker(0) computes and prints the total */
void *Worker(void *arg) {
  long myid = (long) arg;
  int total, i, j, first, last, min, max;
  int lMinPos[2];
  int lMaxPos[2];

#ifdef DEBUG
  printf("worker %d (pthread id %u) has started\n", myid, pthread_self());
#endif

  /* determine first and last rows of my strip */
  first = myid*stripSize;
  last = (myid == numWorkers - 1) ? (size - 1) : (first + stripSize - 1);

  /* sum values in my strip and find the min and max values */
  total = 0;
  min = max = matrix[first][0];
  lMinPos[0] = first; lMinPos[1] = 0;
  lMaxPos[0] = first; lMaxPos[1] = 0;
  for (i = first; i <= last; i++)
    for (j = 0; j < size; j++) {
      total += matrix[i][j];
      if (matrix[i][j] > max) {
        max = matrix[i][j];
        lMaxPos[0] = i; lMaxPos[1] = j;
      }
      else if (matrix[i][j] < min) {
        min = matrix[i][j];
        lMinPos[0] = i; lMinPos[1] = j;
      }
    }
  sums[myid] = total;
  maxes[myid] = max;
  mins[myid] = min;
  maxesPos[myid][0] = lMaxPos[0]; maxesPos[myid][1] = lMaxPos[1];
  minsPos[myid][0] = lMinPos[0]; minsPos[myid][1] = lMinPos[1];
  Barrier();
  if (myid == 0) {
    total = 0;
    min = mins[0];
    max = maxes[0];
    maxPos[0] = maxesPos[0][0]; maxPos[1] = maxesPos[0][1];
    minPos[0] = minsPos[0][0]; minPos[1] = minsPos[0][1];
    for (i = 0; i < numWorkers; i++) {
      total += sums[i];
      if (maxes[i] > max) {
        max = maxes[i];
        maxPos[0] = maxesPos[i][0]; maxPos[1] = maxesPos[i][1];
      }
      if (mins[i] < min) {
        min = mins[i];
        minPos[0] = minsPos[i][0]; minPos[1] = minsPos[i][1];
      }
    }
    /* get end time */
    end_time = read_timer();
    /* print results */
    printf("The total is %d\n", total);
    printf("The min is %d at position (%d,%d)\n", min, minPos[0], minPos[1]);
    printf("The max is %d at position (%d,%d)\n", max, maxPos[0], maxPos[1]);
    printf("The execution time is %g sec\n", end_time - start_time);
  }
}
