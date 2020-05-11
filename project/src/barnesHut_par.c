#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "aux.h"
#include "quadtree.h"

#define G 6.67e-5
#define MAXBODIES 250
#define LENGTH 1.0
#define MAXMASS 1.0
#define MINMASS 1.0

#define GNUMBODIES_DEFAULT 20
#define NUMSTEPS_DEFAULT 100
#define FAR_DEFAULT 1
#define NUMWORKERS_DEFAULT 1

#define PRINT

/**
  * The program takes as arguments: number of bodies, number of steps, far, number of workers
  */
void main(int argc, char *argv[]) {
  quadtree_body_t bodies[MAXBODIES];
  double dt = 1;

  int gnumBodies = GNUMBODIES_DEFAULT;
  int numSteps = NUMSTEPS_DEFAULT;
  double far = FAR_DEFAULT;
  int numWorkers = NUMWORKERS_DEFAULT;

  if(argc > 5) {
    printf("Too many arguments! Ignoring extra arguments\n");
    gnumBodies = (int)strtol(argv[1], NULL, 10);
    numSteps = (int)strtol(argv[2], NULL, 10);
    far = atof(argv[3]);
    numWorkers = (int)strtol(argv[4], NULL, 10);
  } else if(argc == 5) {
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

#ifdef PRINT
  FILE *fp = fopen("data/positions.csv", "w");
#endif
  printf("Args: n=%d len=%f, minMass=%f, maxMass=%f\n", gnumBodies, LENGTH, MINMASS, MAXMASS);
  init_bodies(gnumBodies, LENGTH, MINMASS, MAXMASS, bodies);
#ifdef PRINT
  write_positions(fp, n, bodies);
#endif
  int i = 0;
  for (double t = 0.0; t < numSteps; t+=dt) {
    quadtree_t *tree = quadtree_new(LENGTH, far);
    for(i = 0; i < n; i++) {
      insert_body(tree, &bodies[i]);
    }
    for(i = 0; i < n; i++) {
      update_force(tree, &bodies[i], G);
    }
    move_bodies(bodies, n, dt);
#ifdef PRINT
    write_positions(fp, n, bodies);
#endif
    quadtree_free(tree);
  }
#ifdef PRINT
  fclose(fp);
#endif
}
