#ifndef __QUADTREE_H__
#define __QUADTREE_H__

#include <stdlib.h>
#include <math.h>

typedef struct quadtree_quad {
	double x;
	double y;
	double length;
} quadtree_quad_t;

typedef struct quadtree_body {
	double p_x;
	double p_y;
	double v_x;
	double v_y;
	double f_x;
	double f_y;
	double mass;
} quadtree_body_t;

typedef struct quadtree_node {
	struct quadtree_node *nw;
	struct quadtree_node *ne;
	struct quadtree_node *se;
	struct quadtree_node *sw;
	struct quadtree_quad *quad;
	struct quadtree_body *body;
	double p_x;
	double p_y;
	double v_x;
	double v_y;
	double f_x;
	double f_y;
	double mass;
} quadtree_node_t;

typedef struct quadtree {
	struct quadtree_node *root;
	double threshold;
} quadtree_t;

quadtree_body_t*
quadtree_body_new(double p_x, double p_y,
	 								double v_x, double v_y,
									double f_x, double f_y,
									double mass);

void
quadtree_body_free(quadtree_body_t *body);

quadtree_quad_t*
quadtree_quad_new(double x, double y, double length);

void
quadtree_quad_free(quadtree_quad_t *quad);

quadtree_node_t*
quadtree_node_new();

void
quadtree_node_free(quadtree_node_t *node);

quadtree_t*
quadtree_new(double length, double threshold);

void
quadtree_free(quadtree_t *tree);

/**
 	* Returns true if the quadrant contains the body
	*/
int
node_contains_body(quadtree_quad_t *quad, quadtree_body_t *body);

int
node_is_empty(quadtree_node_t *node);

int
node_is_leaf(quadtree_node_t *node);

int
insert_body(quadtree_t *tree, quadtree_body_t *body);
/**
	* Calculates the euclidean distance between bodies a and b
	*/
double
distance_between_bodies(quadtree_body_t *a, quadtree_body_t *b);
/**
	* Updates the center of mass for a node with a body
	*/
void
update_center_of_mass(quadtree_node_t *node, quadtree_body_t *body);
/**
	* Inserts a body into the tree
	*/
int
insert_body(quadtree_t *tree, quadtree_body_t *body);
/**
	* Updates the forces acting on a given body
	*/
void
update_force(quadtree_t *tree, quadtree_body_t *body, double g);
/**
	* Tests if the far value (threshold) gives that 80% of bodies will used
	* approximation
	*/
int
test_far_value(quadtree_t *tree, int n);

#endif
