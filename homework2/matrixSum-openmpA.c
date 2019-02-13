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

/* structs */
struct Position { int x; int y; };
struct Compare { int val; struct Position pos; };

/* read command line, initialize, and create threads */
int main(int argc, char *argv[]) {
  int i, j, total=0;

  /* read command line args if any */
  size = (argc > 1)? atoi(argv[1]) : MAXSIZE;
  numWorkers = (argc > 2)? atoi(argv[2]) : MAXWORKERS;
  if (size > MAXSIZE) size = MAXSIZE;
  if (numWorkers > MAXWORKERS) numWorkers = MAXWORKERS;

  omp_set_num_threads(numWorkers);

  /* initialize the matrix */
  for (i = 0; i < size; i++) {
      //printf("[ ");
	  for (j = 0; j < size; j++) {
      matrix[i][j] = rand()%99;
      	  //printf(" %d", matrix[i][j]);
	  }
	  	  //printf(" ]\n");
  }

  /* User defined reduction that takes a Compare struct as input */
  #pragma omp declare reduction(maximum : struct Compare :        \
          omp_out = omp_in.val > omp_out.val ? omp_in : omp_out)  \
          initializer (omp_priv = omp_orig)

  #pragma omp declare reduction(minimum : struct Compare :        \
          omp_out = omp_in.val < omp_out.val ? omp_in : omp_out)  \
          initializer (omp_priv = omp_orig)

  /* initialize the compare structs */
  struct Compare max = { .val = matrix[0][0], .pos = { .x = 0, .y = 0} };
  struct Compare min = { .val = matrix[0][0], .pos = { .x = 0, .y = 0} };

  start_time = omp_get_wtime();
#pragma omp parallel for reduction(+:total) reduction(minimum:min) reduction(maximum:max) \
                          private(j)
for (i = 0; i < size; i++)
    for (j = 0; j < size; j++){
      total += matrix[i][j];
      if (matrix[i][j] < min.val) {
        min.val = matrix[i][j];
        min.pos.x = i;
        min.pos.y = j;
      } else if (matrix[i][j] > max.val) {
        max.val = matrix[i][j];
        max.pos.x = i;
        max.pos.y = j;
      }
    }
// implicit barrier

  end_time = omp_get_wtime();

  printf("the total is %d\n", total);
  printf("the max is %d at position (%d, %d)\n", max.val, max.pos.x, max.pos.y);
  printf("the min is %d at position (%d, %d)\n", min.val, min.pos.x, min.pos.y);
  printf("it took %g seconds\n", end_time - start_time);
}
