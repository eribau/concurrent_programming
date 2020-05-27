#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "aux.h"
#include "quadtree.h"
#include "conf.h"

#define G 6.67e-5
#define MAXBODIES 250
#define LENGTH 1.0
#define MAXMASS 1e-4
#define MINMASS 1e-4

#define GNUMBODIES_DEFAULT 20
#define NUMSTEPS_DEFAULT 100
#define FAR_DEFAULT 0.25

// #define LOG
// #define TESTOUTPUT

/**
  * The program takes as arguments: number of bodies, number of steps, far
  */
void main(int argc, char *argv[]) {
  quadtree_body_t bodies[MAXBODIES];
  double start_time, end_time;
  double dt = 1;

  int gnumBodies = GNUMBODIES_DEFAULT;
  int numSteps = NUMSTEPS_DEFAULT;
  double far = FAR_DEFAULT;

  if(argc > 4) {
    printf("Too many arguments! Ignoring extra arguments\n");
    gnumBodies = (int)strtol(argv[1], NULL, 10);
    numSteps = (int)strtol(argv[2], NULL, 10);
    far = atof(argv[3]);
  }  else if(argc == 4) {
    gnumBodies = (int)strtol(argv[1], NULL, 10);
    numSteps = (int)strtol(argv[2], NULL, 10);
    far = atof(argv[3]);
  } else if(argc == 3) {
    gnumBodies = (int)strtol(argv[1], NULL, 10);
    numSteps = (int)strtol(argv[2], NULL, 10);
  } else if(argc == 2){
    gnumBodies = (int)strtol(argv[1], NULL, 10);
  }

  int n =  gnumBodies;

#ifdef LOG
  FILE *fp = fopen("data/positions.csv", "w");
#endif

  // Initialize bodies
  init_bodies(gnumBodies, LENGTH, MINMASS, MAXMASS, bodies);
#ifdef LOG
  // Print inital positions
  write_positions(fp, n, bodies);
#endif

  int i = 0;
  start_time = read_timer();
  for (double t = 0.0; t < numSteps; t+=dt) {
    quadtree_t *tree = quadtree_new(LENGTH, far);
    for(i = 0; i < n; i++) {
      insert_body(tree, &bodies[i]);
    }
    for(i = 0; i < n; i++) {
      update_force(tree, &bodies[i], G);
    }
    move_bodies(bodies, n, dt);
#ifdef LOG
    write_positions(fp, n, bodies);
#endif
    quadtree_free(tree);
  }
  end_time = read_timer();


#ifdef LOG
  fclose(fp);
#endif

#ifndef TESTOUTPUT
  printf("Args: n=%d steps=%d, far=%f Time: %f\n", n, numSteps, far, end_time - start_time);
#else
  printf("%f\n", end_time- start_time);
#endif
}
