#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define G 6.67e-11

struct Point {
  double x;
  double y;
};

int n;                // number of bodies
Point p[], v[], f[];  // position, velocity and force of each body
double m[];           // mass for each body
double dt;            // delta t, timesteps

void calculateForces() {
  double distance, magnitue;
  Point direction;

  for (int i = 0; i < n-1; i++) {
    for (int j = i + 1; j < n; j++) {
      distance = sqrt((p[i].x - p[j].x)^2 + (p[i].y - p[j].y)^2);
      magnitue = (G*m[i]*m[j]) / distance^2;
      direction = { .x = (p[j].x - p[i].x),
                    .y = (p[j].y - p[i].y)};
      f[i].x = f[i].x + magnitue*direction.x/distance;
      f[j].x = f[j].x - magnitue*direction.x/distance;
      f[i].y = f[i].y + magnitue*direction.y/distance;
      f[j].y = f[j].y - magnitue*direction.y/distance;
    }
  }
}

void moveBodies() {
  Point deltav;
  Point deltap;
  for (int i = 0; i < n; i++) {
    deltav = {  .x = f[i].x/m[i]*dt,
                .y = f[i].y/m[i].dt};
    deltap = {  .x = (v[i].x + deltav.x/2) * dt,
                .y = (v[i].y + deltav.y/2) * dt};
    v[i].x = v[i].x + deltav.x;
    v[i].y = v[i].y + deltav.y;
    p[i].x = p[i].x + deltap.x;
    p[i].y = p[i].y + deltap.y;
    f[i].x = f[i].y = 0.0;
  }
}

void init() {
  FILE *planets;
  char buf[255];

  planets = fopen("../planets.txt", "r");
  fscanf(fp, )

}

void run_nSqr_seq() {


  for (double t = 0.0; t < 100; t+=dt) {
    calculateForces();
    moveBodies();
  }
}
