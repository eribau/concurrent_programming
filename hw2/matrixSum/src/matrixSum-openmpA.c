/* matrix summation using OpenMP

   usage with gcc (version 4.2 or higher required):
     gcc -O -fopenmp -o matrixSum-openmp matrixSum-openmp.c
     ./matrixSum-openmp size numWorkers

*/

#include <omp.h>

double start_time, end_time;

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define MAXSIZE 10000  /* maximum matrix size */
#define MAXWORKERS 8   /* maximum number of workers */

int numWorkers;
int size;
int matrix[MAXSIZE][MAXSIZE];
void *Worker(void *);

struct Position {
  int x;
  int y;
};

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j, max_val, min_val, total=0;

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  omp_set_num_threads(numWorkers);

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
      printf("[ ");
	  for (j = 0; j < size; j++) {
      matrix[i][j] = rand()%99;
      	  printf(" %d", matrix[i][j]);
	  }
	  	  printf(" ]\n");
  }

  max_val = matrix[0][0];
  min_val = matrix[0][0];
  struct Position max_pos = { .x = 0, .y = 0 };
  struct Position min_pos = { .x = 0, .y = 0 };

  struct Compare { int val; struct Position pos; };

  start_time = omp_get_wtime();
#pragma omp parallel for reduction(+:total) reduction(max:max_val) reduction(min:min_val) \
                          private(j) shared(max_pos, min_pos)
for (i = 0; i < size; i++)
    for (j = 0; j < size; j++){
      total += matrix[i][j];
      if (matrix[i][j] > max_val) {
        max_val = matrix[i][j];
        max_pos.x = i;
        max_pos.y = j;
      } else if (matrix[i][j] < min_val) {
        min_val = matrix[i][j];
        min_pos.x = i;
        min_pos.y = j;
      }
    }
// implicit barrier

  end_time = omp_get_wtime();

  printf("the total is %d\n", total);
  printf("the max is %d at position (%d, %d)\n", max_val, max_pos.x, max_pos.y);
  printf("the min is %d at position (%d, %d)\n", min_val, min_pos.x, min_pos.y);
  printf("it took %g seconds\n", end_time - start_time);

}
