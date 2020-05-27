#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <omp.h>
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
#define NUMWORKERS_DEFAULT 1

// #define LOG
// #define TESTOUTPUT

/**
  * The program takes as arguments: number of bodies, number of steps, far, number of workers
  */
void main(int argc, char *argv[]) {
  quadtree_body_t bodies[MAXBODIES];
  double start_time, end_time;
  double dt = 1;

  int gnumBodies = GNUMBODIES_DEFAULT;
  int numSteps = NUMSTEPS_DEFAULT;
  double far = FAR_DEFAULT;
  int numWorkers = NUMWORKERS_DEFAULT;

  if(argc >= 5) {
    if(argc > 5) {
      printf("Too many arguments! Ignoring extra arguments\n");
    }
    gnumBodies = (int)strtol(argv[1], NULL, 10);
    numSteps = (int)strtol(argv[2], NULL, 10);
    far = atof(argv[3]);
    numWorkers = (int)strtol(argv[4], NULL, 10);
  } else if(argc == 4) {
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

  omp_set_dynamic(0);
  omp_set_num_threads(numWorkers);
  quadtree_t *tree;
  start_time = read_timer();
  for (double t = 0.0; t < numSteps; t+=dt) {
    tree = quadtree_new(LENGTH, far);
    for(int i = 0; i < n; i++) {
      insert_body(tree, &bodies[i]);
    }
#pragma omp parallel
{
#pragma omp for schedule(static)
    for(int i = 0; i < n; i++) {
      update_force(tree, &bodies[i], G);
      // printf("Thread %d\n", omp_get_thread_num());
    }
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
