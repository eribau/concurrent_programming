#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "aux.h"

#include <omp.h>

#define G 1 // 6.67e-11
#define MAXBODIES 250
#define MAXWORKERS 64

struct Point {
  double x;
  double y;
};

int numWorkers;                                         // number of workers
int n;                                                  // number of bodies
struct Point p[MAXBODIES], v[MAXBODIES];                // position, velocity and
struct Point f[MAXWORKERS][MAXBODIES];                  // force of each body
double m[MAXBODIES];                                    // mass for each body
double dt;                                              // delta t, timesteps
double start_time, end_time;                            // start and end time

void calculateForces_par() {
  double distance, magnitue;
  struct Point direction;

  int w;
#pragma omp  for schedule(dynamic, 1)
  for (int i = 0; i < n-1; i++) {
    w = omp_get_thread_num();
    for (int j = i + 1; j < n; j++) {
      distance = sqrt((p[i].x - p[j].x)*(p[i].x - p[j].x) + (p[i].y - p[j].y)*(p[i].y - p[j].y));
      magnitue = (G*m[i]*m[j]) / distance*distance;
      direction = (struct Point) {  .x = (p[j].x - p[i].x),
                                    .y = (p[j].y - p[i].y)};
      f[w][i].x = f[w][i].x + magnitue*direction.x/distance;
      f[w][j].x = f[w][j].x - magnitue*direction.x/distance;
      f[w][i].y = f[w][i].y + magnitue*direction.y/distance;
      f[w][j].y = f[w][j].y - magnitue*direction.y/distance;
    }
    printf("w: %d\n", w);
  }
}


void moveBodies_par() {
  struct Point deltav;
  struct Point deltap;
  struct Point force = { .x = 0.0, .y = 0.0 };
#pragma omp for
  for (int i = 0; i < n; i++) {
    for (int k = 0; k < numWorkers; k++) {
      force.x += f[k][i].x;
      f[k][i].x = 0.0;
      force.y += f[k][i].y;
      f[k][i].y = 0.0;
    }
    deltav = (struct Point) { .x = force.x/m[i] * dt,
                              .y = force.y/m[i] * dt};
    deltap = (struct Point) { .x = (v[i].x + deltav.x/2) * dt,
                              .y = (v[i].y + deltav.y/2) * dt};
    v[i].x = v[i].x + deltav.x;
    v[i].y = v[i].y + deltav.y;
    p[i].x = p[i].x + deltap.x;
    p[i].y = p[i].y + deltap.y;
    force.x = force.y = 0.0;
  }
}

void init_par() {
  double x, y;

  for (int i = 0; i < n; i++) {
    x = cos(2*M_PI*i / n);
    y = sin(2*M_PI*i / n);
    p[i].x = x;
    p[i].y = y;
    v[i].x = (-2*M_PI/n * y);
    v[i].y = (2*M_PI/n * x);
    //f[i].x = -4*M_PI*M_PI/(n*n) * x;
    //f[i].x = -4*M_PI*M_PI/(n*n) * y;
    m[i] = 1;
  }
}

void printBodies_par() {
  printf("planet nr\t position\t velocity\t mass\n");
  for (int i = 0; i < n; i++) {
    printf("%d\t", i);
    printf("(%f,%f)\t", p[i].x, p[i].y);
    printf("(%f,%f)\t", v[i].x, v[i].y);
    printf("%f\n", m[i]);
  }

}

/* run the simulation and return the time it took */
double run_nSqr_par(int gnumBodies, int numSteps, int nw) {
  dt = 1;
  n = gnumBodies;
  numWorkers = nw;

  if (n > MAXBODIES) {
    printf("Number of bodies must be less than %d", MAXBODIES);
    exit(1);
  }
  if (numWorkers > MAXWORKERS) {
    printf("Number of workers must be less than %d", MAXWORKERS);
    exit(1);
  }

  init_par();

  printf("#number of bodies: %d\n", n);
  printf("#number of timesteps: %d\n", numSteps);
  //printBodies();

  start_time = read_timer();
  omp_set_dynamic(0);
  omp_set_num_threads(numWorkers);
#pragma omp parallel
{
  for (double t = 0.0; t < numSteps; t+=dt) {
    calculateForces_par();
    moveBodies_par();
  }
}
  end_time = read_timer();
  printBodies_par();

  return end_time - start_time;
}
