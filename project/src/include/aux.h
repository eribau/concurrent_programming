#include "quadtree.h"

double read_timer();

void init_bodies(int n, double length, double minMass, double maxMass, quadtree_body_t *bodies);

// FILE *init_data_file(const char *filename);

void write_positions(FILE *fp, int n, quadtree_body_t *bodies);
