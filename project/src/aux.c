#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "quadtree.h"

/* timer copied from matrixSum.c */
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

/* Function taken from stackoverflow user rep_movsd, https://stackoverflow.com/questions/2704521/generate-random-double-numbers-in-c */
double fRand(double fMin, double fMax) {
  double f = (double)rand() / RAND_MAX;
  return fMin + f * (fMax - fMin);
}

void init_bodies(int n, double length, double minMass, double maxMass, quadtree_body_t *bodies) {
    srand(42);

    // TODO figure out a good way of initializing bodies, need to consider position as well as initial acceleration and mass
    for(int i = 0; i < n; i++) {
      bodies[i].p_x = fRand(0, length);
      bodies[i].p_y = fRand(0, length);
      bodies[i].v_x = 0;
      bodies[i].v_y = 0;
      bodies[i].f_x = 0;
      bodies[i].f_y = 0;
      bodies[i].mass = fRand(minMass, maxMass);
    }
}

void write_positions(FILE *fp, int n, quadtree_body_t *bodies) {
  // TODO write positional data of bodies to a csv file
  /* position format: x,y where x and y can be 6 char + . + 6 char ex. 123456.002414 */
  int pos_str_len = 30;
  int line_str_len = n * pos_str_len + n;
  char position[pos_str_len];
  char line[line_str_len];

  memset(position, 0, pos_str_len);
  memset(line, 0, line_str_len);

  printf("%s", position);
  for(int i = 0; i < n-1; i++) {
    if(snprintf(position, pos_str_len, "%lf,%lf,", bodies[i].p_x, bodies[i].p_y) >= 0) {
      strcat(line, position);
    } else {
      fprintf(stderr, "!failed to write body position to file\n");
    }
    memset(position, 0, pos_str_len);
  }
  if(snprintf(position, pos_str_len, "%lf,%lf\n", bodies[n-1].p_x, bodies[n-1].p_y) >= 0) {
    strcat(line, position);
  } else {
    fprintf(stderr, "!failed to write body position to file\n");
  }

  fputs(line, fp);
}
