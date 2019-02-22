/* Compile with
  gcc -o quicksort quicksort.c -fopenmp -lm

  Run with
  ./quicksort <size of array> <number of threads>
*/

#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define MAXSIZE 1000000  /* maximum matrix size */
#define MAXWORKERS 32   /* maximum number of workers */
#define TRUE 1
#define FALSE 0

double start_time, end_time;

int numWorkers;
int size;
int array[MAXSIZE];
void *Worker(void *);

void printArray() {
  printf("[ ");
  for (int i = 0; i < size; i++) {
    printf("%d ", array[i]);
  }
  printf("]\n");
}

void swap(int a, int b) {
  int temp = array[a];
  array[a] = array[b];
  array[b] = temp;
}

int partition(int lo, int hi) {
  int pivot = array[(lo + hi)/2];
  int i = lo - 1;
  int j = hi + 1;
  while (TRUE) {
    do {
      i++;
    } while (array[i] < pivot);
    do {
      j--;
    } while (array[j] > pivot);

    if (i >= j)
      return j;

    swap(i, j);
  }
}

void quicksort(int lo, int hi) {
  if (lo < hi) {
    int p = partition(lo, hi);
      quicksort(lo, p);
      #pragma omp task
      quicksort(p + 1, hi);
  }
}


/* test to see if array is sorted or not (descending order; left to right)*/
int isSorted(int *arr, int n) {
  for (int i = 1; i < n; i++) {
    if (arr[i] < arr[i-1]) return 0;
  }
  return 1;
}

int main(int argc, char *argv[]) {
  int i;

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  omp_set_num_threads(numWorkers);

  /* initialize the array */
	  for (i = 0; i < size; i++) {
      array[i] = rand()%99;
  }

  //printArray();
  start_time = omp_get_wtime();
  #pragma omp parallel
  {
    #pragma omp single
    quicksort(0, size-1);
  }
  end_time = omp_get_wtime();
  //printArray();

  assert(isSorted(array, size));

  printf("it took %g seconds\n", end_time - start_time);
}
