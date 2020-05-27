#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "aux.h"
#include "conf.h"

#define G 6.67e-5
#define MAXBODIES 250
#define LENGTH 1.0
#define MAXMASS 1e-4
#define MINMASS 1e-4

#define GNUMBODIES_DEFAULT 20
#define NUMSTEPS_DEFAULT 100
#define NUMWORKERS_DEFAULT 1

// #define LOG
// #define TESTOUTPUT

struct Point {
  double x;
  double y;
};

struct Point p[MAXBODIES], v[MAXBODIES], f[MAXBODIES];  // position, velocity and force of each body
double m[MAXBODIES];                                    // mass for each body
double dt;                                              // delta t, timesteps

void calculateForces_seq(int n, double g) {
  double distance, magnitude;
  struct Point direction;

  for (int i = 0; i < n-1; i++) {
    for (int j = i + 1; j < n; j++) {
      distance = sqrt((p[i].x - p[j].x)*(p[i].x - p[j].x) + (p[i].y - p[j].y)*(p[i].y - p[j].y));
      magnitude = (g*m[i]*m[j]) / (distance*distance);
      direction = (struct Point) { .x = (p[j].x - p[i].x),
                    .y = (p[j].y - p[i].y)};
      f[i].x += magnitude*direction.x;
      f[j].x -= magnitude*direction.x;
      f[i].y += magnitude*direction.y;
      f[j].y -= magnitude*direction.y;
      // printf("g %f dist %f mi %f mj %f mag %.14f %.14f\n", g, distance, m[i], m[j], magnitude, g*m[i]*m[j]);
    }
  }
}

void moveBodies_seq(int n) {
  struct Point deltav;
  struct Point deltap;
  for (int i = 0; i < n; i++) {
    deltav = (struct Point) { .x = f[i].x/m[i] * dt,
                              .y = f[i].y/m[i] * dt};
    deltap = (struct Point) { .x = (v[i].x + deltav.x/2) * dt,
                              .y = (v[i].y + deltav.y/2) * dt};
    // printf("deltav x %.16f\n", deltav.x);
    v[i].x = v[i].x + deltav.x;
    v[i].y = v[i].y + deltav.y;
    p[i].x = p[i].x + deltap.x;
    p[i].y = p[i].y + deltap.y;
    f[i].x = f[i].y = 0.0;
  }
}

void init_seq(int n, double length, double minMass, double maxMass) {
  for (int i = 0; i < n; i++) {
    p[i].x = fRand(0, length);
    p[i].y = fRand(0, length);
    v[i].x = 0;
    v[i].y = 0;
    f[i].x = 0;
    f[i].x = 0;
    m[i] = fRand(minMass, maxMass);
  }
}

void write_positions_seq(FILE *fp, int n) {
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
  * The program takes as arguments: number of bodies, number of steps, far
  */
void main(int argc, char *argv[]) {
  double start_time, end_time;
  dt = 1;

  int gnumBodies = GNUMBODIES_DEFAULT;
  int numSteps = NUMSTEPS_DEFAULT;
  int numWorkers = NUMWORKERS_DEFAULT;

  if(argc >= 3) {
    if(argc > 3) {
      printf("Too many arguments! Ignoring extra arguments\n");
    }
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
  init_seq(n, LENGTH, MINMASS, MAXMASS);
#ifdef LOG
  // Print inital positions
  write_positions_seq(fp, n);
#endif

  start_time = read_timer();
  for (double t = 0.0; t < numSteps; t+=dt) {
    calculateForces_seq(n, g);
    moveBodies_seq(n);
#ifdef LOG
    // Print inital positions
    write_positions_seq(fp, n);
#endif
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
