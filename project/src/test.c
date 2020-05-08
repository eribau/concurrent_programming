#include <assert.h>
#include <stdio.h>

#include "aux.h"
#include "quadtree.h"

#define test(fn) \
        printf("\x1b[33m" # fn "\x1b[0m "); \
        test_##fn(); \
        puts("\x1b[1;32m ✓ \x1b[0m");

static void
test_quad() {
  quadtree_quad_t *quad = quadtree_quad_new(0.0, 0.0, 1.0);
  assert(quad->x == 0.0);
  assert(quad->y == 0.0);
  assert(quad->length == 1.0);
  quadtree_quad_free(quad);
}

static void
test_body() {
  quadtree_body_t *body = quadtree_body_new(1.0, 1.0,
                                            1.0, 1.0,
                                            1.0, 1.0,
                                            1.0);
  assert(body->p_x == 1.0);
  assert(body->p_y == 1.0);
  assert(body->v_x == 1.0);
  assert(body->v_y == 1.0);
  assert(body->f_x == 1.0);
  assert(body->f_y == 1.0);
  assert(body->mass == 1.0);
  quadtree_body_free(body);
}

static void
test_node() {
  quadtree_node_t *node = quadtree_node_new();
  assert(node->nw == NULL);
  assert(node->ne == NULL);
  assert(node->sw == NULL);
  assert(node->se == NULL);
  assert(node->quad == NULL);
  assert(node->body == NULL);
  quadtree_node_free(node);
}

static void
test_distance_between_bodies() {
  quadtree_body_t *body1 = quadtree_body_new(0.0, 0.0,
                                              1.0, 1.0,
                                              1.0, 1.0,
                                              1.0);
  quadtree_body_t *body2 = quadtree_body_new(4.0, 3.0,
                                              1.0, 1.0,
                                              1.0, 1.0,
                                              1.0);
  assert(distance_between_bodies(body1, body2) == 5.0);
  quadtree_body_free(body1);
  quadtree_body_free(body2);
}

static void
test_update_center_of_mass() {
  quadtree_node_t *node = quadtree_node_new();
  node->p_x = 0.0;
  node->p_y = 0.0;
  node->mass = 1.0;
  quadtree_body_t *body = quadtree_body_new(1.0, 1.0,
                                            1.0, 1.0,
                                            1.0, 1.0,
                                            1.0);
  update_center_of_mass(node, body);
  assert(node->mass == 2.0);
  assert(node->p_x == 0.5);
  assert(node->p_y == 0.5);
  quadtree_node_free(node);
  quadtree_body_free(body);
}

static void
test_insert_one_body() {
  quadtree_t *tree = quadtree_new(1.0, 0.5);
  quadtree_body_t *body = quadtree_body_new(0.5, 0.5,
                                            1.0, 1.0,
                                            1.0, 1.0,
                                            1.0);
  insert_body(tree, body);
  assert(tree->root->body != NULL);
  assert(tree->root->body->p_x == 0.5);
  assert(tree->root->body->p_y == 0.5);
  assert(tree->root->body->mass == 1.0);
  quadtree_free(tree);
  quadtree_body_free(body);
}

static void
test_insert_two_bodies() {
  quadtree_t *tree = quadtree_new(1.0, 0.5);
  quadtree_body_t *body1 = quadtree_body_new(0.25, 0.25,
                                              1.0, 1.0,
                                              1.0, 1.0,
                                              1.0);
  quadtree_body_t *body2 = quadtree_body_new(0.75, 0.75,
                                              1.0, 1.0,
                                              1.0, 1.0,
                                              1.0);
  insert_body(tree, body1);
  insert_body(tree, body2);
  assert(tree->root->body == NULL);
  assert(tree->root->p_x == 0.5);
  assert(tree->root->p_y == 0.5);
  assert(tree->root->mass == 2.0);
  assert(tree->root->sw->body == body1);
  assert(tree->root->sw->p_x == 0.25);
  assert(tree->root->sw->p_y == 0.25);
  assert(tree->root->sw->mass == 1.0);
  assert(tree->root->ne->body == body2);
  assert(tree->root->ne->p_x == 0.75);
  assert(tree->root->ne->p_y == 0.75);
  assert(tree->root->ne->mass == 1.0);
  quadtree_free(tree);
  quadtree_body_free(body1);
  quadtree_body_free(body2);
}

static void
test_insert_three_bodies() {
  quadtree_t *tree = quadtree_new(1.0, 0.5);
  quadtree_body_t *body1 = quadtree_body_new(0.25, 0.25,
                                              1.0, 1.0,
                                              1.0, 1.0,
                                              1.0);
  quadtree_body_t *body2 = quadtree_body_new(0.75, 0.75,
                                              1.0, 1.0,
                                              1.0, 1.0,
                                              1.0);
  quadtree_body_t *body3 = quadtree_body_new(0.95, 0.95,
                                              1.0, 1.0,
                                              1.0, 1.0,
                                              1.0);
  insert_body(tree, body1);
  insert_body(tree, body2);
  insert_body(tree, body3);
  // Root should be empty but have the total sum of masses and average position
  assert(tree->root->body == NULL);
  assert(tree->root->p_x == 0.65);
  assert(tree->root->p_y == 0.65);
  assert(tree->root->mass == 3.0);
  // SW of root should contain body1
  assert(tree->root->sw->body == body1);
  assert(tree->root->sw->p_x == 0.25);
  assert(tree->root->sw->p_y == 0.25);
  assert(tree->root->sw->mass == 1.0);
  // NE of root should be empty but have the total sum of masses for
  // body2 and body3
  assert(tree->root->ne->body == NULL);
  assert(tree->root->ne->p_x == 0.85);
  assert(tree->root->ne->p_y == 0.85);
  assert(tree->root->ne->mass == 2.0);
  // NW of NE of root should contain body2, since if a point is
  // exactly on a gridline it will be put into the one closest
  // to NW going clockwise
  assert(tree->root->ne->nw->body == body2);
  assert(tree->root->ne->nw->p_x == 0.75);
  assert(tree->root->ne->nw->p_y == 0.75);
  assert(tree->root->ne->nw->mass == 1.0);
  // NE of NE of root shoudl contain body3
  assert(tree->root->ne->ne->body == body3);
  assert(tree->root->ne->ne->p_x == 0.95);
  assert(tree->root->ne->ne->p_y == 0.95);
  assert(tree->root->ne->ne->mass == 1.0);
  // check to see if SE and NW of root are empty
  assert(node_is_empty(tree->root->se));
  assert(node_is_empty(tree->root->nw));
  quadtree_free(tree);
  quadtree_body_free(body1);
  quadtree_body_free(body2);
  quadtree_body_free(body3);
}

static void
test_update_force() {
  quadtree_t *tree = quadtree_new(1.0, 0.5);
  quadtree_body_t *body1 = quadtree_body_new(0.0, 0.0,
                                              1.0, 1.0,
                                              0.0, 0.0,
                                              1.0);
  quadtree_body_t *body2 = quadtree_body_new(0.4, 0.3,
                                              1.0, 1.0,
                                              0.0, 0.0,
                                              1.0);
  insert_body(tree, body1);
  insert_body(tree, body2);
  update_force(tree, body1, 1.0);
  assert(body1->f_x == 3.2);
  assert(body1->f_y == 2.4);
  quadtree_free(tree);
  quadtree_body_free(body1);
  quadtree_body_free(body2);
}

static void
test_test_far_value() {
  int n = 120;
  double length = 1.0;
  double far = 0.25;

  quadtree_body_t bodies[n];
  init_bodies(n, length, 1.0, 1.0, bodies);
  quadtree_t *tree = quadtree_new(length, far);
  for(int i = 0; i < n; i++) {
    insert_body(tree, &bodies[i]);
  }
  int result = test_far_value(tree, n, bodies);
  assert(result == 1);
  quadtree_free(tree);
  // for(int i = 0; i < n; i++) {
  //   quadtree_body_free(&bodies[i]);
  // }
}

int
main(int argc, const char *argv[]) {
  test(quad);
  test(node);
  test(distance_between_bodies);
  test(update_center_of_mass);
  test(insert_one_body);
  test(insert_two_bodies);
  test(insert_three_bodies);
  test(update_force);
  test(test_far_value);
}
