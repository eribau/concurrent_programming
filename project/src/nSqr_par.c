#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "aux.h"
#include "conf.h"

#include <omp.h>

#define G 6.67e-5
#define MAXBODIES 250
#define MAXWORKERS 4
#define LENGTH 1.0
#define MAXMASS 1e-4
#define MINMASS 1e-4

#define GNUMBODIES_DEFAULT 20
#define NUMSTEPS_DEFAULT 100
#define FAR_DEFAULT 1
#define NUMWORKERS_DEFAULT 1

// #define LOG
#define TESTOUTPUT

struct Point {
  double x;
  double y;
};

int numWorkers;                                         // number of workers
struct Point p[MAXBODIES], v[MAXBODIES];                // position, velocity and
struct Point f[MAXWORKERS][MAXBODIES];                  // force of each body
double m[MAXBODIES];                                    // mass for each body
double dt;                                              // delta t, timesteps
double start_time, end_time;                            // start and end time

void calculateForces_par(int n, double g) {
  double distance, magnitue;
  struct Point direction;

  int w;
#pragma omp for schedule(dynamic, 1)
  for (int i = 0; i < n-1; i++) {
    w = omp_get_thread_num();
    for (int j = i + 1; j < n; j++) {
      distance = sqrt((p[i].x - p[j].x)*(p[i].x - p[j].x) + (p[i].y - p[j].y)*(p[i].y - p[j].y));
      magnitue = (g*m[i]*m[j]) / (distance*distance);
      direction = (struct Point) {  .x = (p[j].x - p[i].x),
                                    .y = (p[j].y - p[i].y)};
      f[w][i].x = f[w][i].x + magnitue*direction.x/distance;
      f[w][j].x = f[w][j].x - magnitue*direction.x/distance;
      f[w][i].y = f[w][i].y + magnitue*direction.y/distance;
      f[w][j].y = f[w][j].y - magnitue*direction.y/distance;
    }
  }
}


void moveBodies_par(int n) {
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

void init_par(int n, double length, double minMass, double maxMass) {
  for (int i = 0; i < n; i++) {
    p[i].x = fRand(0, length);
    p[i].y = fRand(0, length);
    v[i].x = 0;
    v[i].y = 0;
    m[i] = fRand(minMass, maxMass);
  }
}

void write_positions_par(FILE *fp, int n) {
  /* position format: x,y where x and y can be 6 char + . + 6 char ex. 123456.002414 */
  int pos_str_len = 30;
  int line_str_len = n * pos_str_len + n;
  char position[pos_str_len];
  char line[line_str_len];

  memset(position, 0, pos_str_len);
  memset(line, 0, line_str_len);

  printf("%s", position);
  for(int i = 0; i < n-1; i++) {
    if(snprintf(position, pos_str_len, "%lf,%lf,", p[i].x, p[i].y) >= 0) {
      strcat(line, position);
    } else {
      fprintf(stderr, "!failed to write body position to file\n");
    }
    memset(position, 0, pos_str_len);
  }
  if(snprintf(position, pos_str_len, "%lf,%lf\n", p[n-1].x, p[n-1].y) >= 0) {
    strcat(line, position);
  } else {
    fprintf(stderr, "!failed to write body position to file\n");
  }

  fputs(line, fp);
}

/**
  * The program takes as arguments: number of bodies, number of steps, number of workers
  */
void main(int argc, char *argv[]) {
  double start_time, end_time;
  dt = 1;

  int gnumBodies = GNUMBODIES_DEFAULT;
  int numSteps = NUMSTEPS_DEFAULT;
  int numWorkers = NUMWORKERS_DEFAULT;

  if(argc >= 4) {
    if(argc > 4) {
      printf("Too many arguments! Ignoring extra arguments\n");
    }
    gnumBodies = (int)strtol(argv[1], NULL, 10);
    numSteps = (int)strtol(argv[2], NULL, 10);
    numWorkers = (int)strtol(argv[3], NULL, 10);
  } else if(argc == 3) {
    gnumBodies = (int)strtol(argv[1], NULL, 10);
    numSteps = (int)strtol(argv[2], NULL, 10);
  } else if(argc == 2) {
    gnumBodies = (int)strtol(argv[1], NULL, 10);
  }

  int n =  gnumBodies;
  double g = G;

#ifdef LOG
  FILE *fp = fopen("data/positions.csv", "w");
#endif

  // Initialize bodies
  init_par(gnumBodies, LENGTH, MINMASS, MAXMASS);
#ifdef LOG
  // Print inital positions
  write_positions_par(fp, n);
#endif

  start_time = read_timer();
  omp_set_dynamic(0);
  omp_set_num_threads(numWorkers);
  #pragma omp parallel
  {
    for (double t = 0.0; t < numSteps; t+=dt) {
      calculateForces_par(n, g);
      moveBodies_par(n);
#ifdef LOG
      // Print inital positions
      write_positions_par(fp, n);
#endif
    }
  }
  end_time = read_timer();

#ifdef LOG
  fclose(fp);
#endif

#ifndef TESTOUTPUT
  printf("Args: n=%d steps=%d, Time: %f\n", n, numSteps, end_time - start_time);
#else
  printf("%f\n", end_time- start_time);
#endif
}
