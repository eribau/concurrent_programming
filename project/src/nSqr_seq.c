#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "aux.h"
#define G 1 // 6.67e-11
#define MAXBODIES 250

struct Point {
  double x;
  double y;
};

int n;                                                  // number of bodies
struct Point p[MAXBODIES], v[MAXBODIES], f[MAXBODIES];  // position, velocity and force of each body
double m[MAXBODIES];                                    // mass for each body
double dt;                                              // delta t, timesteps
double start_time, end_time;                            // start and end time

void calculateForces_seq() {
  double distance, magnitue;
  struct Point direction;

  for (int i = 0; i < n-1; i++) {
    for (int j = i + 1; j < n; j++) {
      distance = sqrt((p[i].x - p[j].x)*(p[i].x - p[j].x) + (p[i].y - p[j].y)*(p[i].y - p[j].y));
      magnitue = (G*m[i]*m[j]) / distance*distance;
      direction = (struct Point) { .x = (p[j].x - p[i].x),
                    .y = (p[j].y - p[i].y)};
      f[i].x = f[i].x + magnitue*direction.x/distance;
      f[j].x = f[j].x - magnitue*direction.x/distance;
      f[i].y = f[i].y + magnitue*direction.y/distance;
      f[j].y = f[j].y - magnitue*direction.y/distance;
    }
  }
}

void moveBodies_seq() {
  struct Point deltav;
  struct Point deltap;
  for (int i = 0; i < n; i++) {
    deltav = (struct Point) { .x = f[i].x/m[i] * dt,
                              .y = f[i].y/m[i] * dt};
    deltap = (struct Point) { .x = (v[i].x + deltav.x/2) * dt,
                              .y = (v[i].y + deltav.y/2) * dt};
    v[i].x = v[i].x + deltav.x;
    v[i].y = v[i].y + deltav.y;
    p[i].x = p[i].x + deltap.x;
    p[i].y = p[i].y + deltap.y;
    f[i].x = f[i].y = 0.0;
  }
}

void init_seq() {
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

void printBodies_seq() {
  printf("planet nr\t position\t velocity\t mass\n");
  for (int i = 0; i < n; i++) {
    printf("%d\t", i);
    printf("(%f,%f)\t", p[i].x, p[i].y);
    printf("(%f,%f)\t", v[i].x, v[i].y);
    printf("%f\n", m[i]);
  }

}

/* run the simulation and return the time it took */
double run_nSqr_seq(int gnumBodies, int numSteps) {
  dt = 1;
  n = gnumBodies;

  if (n > MAXBODIES) {
    printf("Number of bodies must be less than %d", MAXBODIES);
    exit(1);
  }

  init_seq();

  printf("#number of bodies: %d\n", n);
  printf("#number of timesteps: %d\n", numSteps);
  //printBodies();

  start_time = read_timer();
  for (double t = 0.0; t < numSteps; t+=dt) {
    calculateForces_seq();
    moveBodies_seq();
  }
  end_time = read_timer();
  printBodies_seq();

  return end_time - start_time;
}
