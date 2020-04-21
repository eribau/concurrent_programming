#include <stdlib.h>
#include <stdio.h>
#include "nSqr_seq.h"
#include "nSqr_par.h"

#define GNUMBODIES_DEFAULT 25
#define NUMSTEPS_DEFAULT 10
#define FAR_DEFAULT 1
#define NUMWORKERS_DEFAULT 1

// TODO
// Fix inputs: gnumBodies, numSteps, far (distance used to decide when to approximate), numWorkers
// Fix initialization of bodies, should be able to verify correctness
// Implement Barnes-Hut program
// Implement saving of position data so that it can be plotted
// Change driver to perform experiment and save results (?)

void main(int argc, char *argv) {
  double time;

  int gnumBodies = GNUMBODIES_DEFAULT;
  int numSteps = NUMSTEPS_DEFAULT;
  double far = DISTANCE_DEFAULT;
  int numWorkers = NUMWORKERS_DEFAULT;

  if(argc > 5) {
    printf("Too many arguments! Ignoring extra arguments\n");
    gnumBodies = argv[1];
    numSteps = argv[2];
    far = argv[3];
    numWorkers = argv[4];
  } else if(argc == 5) {
    gnumBodies = argv[1];
    numSteps = argv[2];
    far = argv[3];
    numWorkers = argv[4];
  } else if(argc == 4) {
    gnumBodies = argv[1];
    numSteps = argv[2];
    far = argv[3];
  } else if(argc == 3) {
    gnumBodies = argv[1];
    numSteps = argv[2];
  } else if(argc == 2){
    gnumBodies = argv[1];
  }

  time = run_nSqr_seq(3, 1);
  printf("time: %f\n", time);
  time = run_nSqr_par(3, 1, 4);
  printf("time: %f\n", time);
}
