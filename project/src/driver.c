#include <stdlib.h>
#include <stdio.h>
#include "nSqr_seq.h"

void main() {
  double time = run_nSqr_seq(120, 1000);
  printf("time: %f\n", time);
}
