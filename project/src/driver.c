#include <stdlib.h>
#include <stdio.h>
#include "nSqr_seq.h"
#include "nSqr_par.h"

void main() {
  double time;

  time = run_nSqr_seq(3, 1);
  printf("time: %f\n", time);
  time = run_nSqr_par(3, 1, 4);
  printf("time: %f\n", time);
}
