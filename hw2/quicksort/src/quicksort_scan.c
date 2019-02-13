#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "taskQueue.h"

#define MAXSIZE 100  /* maximum matrix size */
#define MAXWORKERS 8   /* maximum number of workers */
#define TRUE 1
#define FALSE 0

double start_time, end_time;

int numWorkers;
int size;
int array[MAXSIZE];
int tCount; /* shared counter */
int done; /* flag to set when done */

void *Worker(void *);

void printArray() {
  printf("[ ");
  for (int i = 0; i < size; i++) {
    printf("%d ", array[i]);
  }
  printf("]\n");
}

//TODO
void prefixScan(int output[], int input[]) {
  for (i = 0; i < log2(size); i++) {
    #pragma omp for
    for (j = 1; j < size; j++) {
      if (j < 1<<i) {}
    }
  }
}

void quicksort() {
  int p, r, q;
  struct IndexPair d, d1, d2;

  int LT[size];
  int GT[size];
  int B[size];
  int C[size];
  int Ld[size];
  int Gd[size];

  while(TRUE) {
    while (isEmpty()) {
      if (done) return;
    }
    d = get();
    p =d.lo;
    r = d.hi;

    if (p < r) {
      int *At = &array[p];
      int pivot = At[0];
      int n = r - p + 1;

      #pragma omp for
      for (int i = 0; i < size; ++i) {
        if (At[i] < pivot) {
          LT[i] = 1;
          GT[i] = 0;
        } else {
          LT[i] = 0;
          GT[i] = 1;
        }
      }
      LT[0] = GT[0] = -1;

      //Scan LT and BT

      int ll = B[n - 1] + 1;
      int gl = C[n - 1] + 1;

      #pragma omp for
      for(int i = 0; i < size; ++i) {
        if (LT[i])
          Ld[B[i]] = At[i];
        else
          Gd[C[i]] = At[i];
      }
      q = ll + p;

      for (int i = 0; i < ll; ++i) {
        At[i] = Ld[i];
      }
      At[ll++] = pivot;
      for (int i = 0; i < gl; ++i) {
        At[ll + i] = Gl[i];
      }
      #pragma omp atomic
      ++tCount;
      d1.lo = p;
      d1.hi = q - 1;
      put(d1);
      d2.lo = q + 1;
      d2.hi = r;
      put(d2);

    } else if (p == r) {
      #pragma omp atomic
      ++tCount;
      if (tCount == size) done = TRUE;
    }
  }
}

int main(int argc, char *argv[]) {
  int i;

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  omp_set_num_threads(numWorkers);

  /* initialize the matrix */
	  for (i = 0; i < size; i++) {
      array[i] = rand()%99;
  }

  /* initialize the queue and variables*/
  done = 0;
  init();

  printArray();
  quicksort();
  printArray();

}
