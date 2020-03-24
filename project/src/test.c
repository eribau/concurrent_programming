#include <assert.h>
#include <stdio.h>

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
  quadtree_body_t *body = quadtree_body_new(1.0, 1.0, 1.0);
  assert(body->x == 1.0);
  assert(body->y == 1.0);
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
  quadtree_body_t *body1 = quadtree_body_new(0.0, 0.0, 1.0);
  quadtree_body_t *body2 = quadtree_body_new(4.0, 3.0, 1.0);
  assert(distance_between_bodies(body1, body2) == 5.0);
  quadtree_body_free(body1);
  quadtree_body_free(body2);
}

static void
test_update_center_of_mass() {
  quadtree_node_t *node = quadtree_node_new();
  node->x = 0.0;
  node->y = 0.0;
  node->mass = 1.0;
  quadtree_body_t *body = quadtree_body_new(1.0, 1.0, 1.0);
  update_center_of_mass(node, body);
  assert(node->mass == 2.0);
  assert(node->x == 0.5);
  assert(node->y == 0.5);
  quadtree_node_free(node);
  quadtree_body_free(body);
}

static void
test_insert_one_body() {
  quadtree_t *tree = quadtree_new(1.0, 0.5);
  quadtree_body_t *body = quadtree_body_new(0.5, 0.5, 1.0);
  insert_body(tree, body);
  assert(tree->root->body != NULL);
  assert(tree->root->body->x == 0.5);
  assert(tree->root->body->y == 0.5);
  assert(tree->root->body->mass == 1.0);
  quadtree_free(tree);
}

static void
test_insert_two_bodies() {
  quadtree_t *tree = quadtree_new(1.0, 0.5);
  quadtree_body_t *body1 = quadtree_body_new(0.25, 0.25, 1.0);
  quadtree_body_t *body2 = quadtree_body_new(0.75, 0.75, 1.0);
  insert_body(tree, body1);
  insert_body(tree, body2);
  assert(tree->root->body == NULL);
  assert(tree->root->x == 0.5);
  assert(tree->root->y == 0.5);
  assert(tree->root->mass == 2.0);
  assert(tree->root->sw->body == body1);
  assert(tree->root->sw->x == 0.25);
  assert(tree->root->sw->y == 0.25);
  assert(tree->root->sw->mass == 1.0);
  assert(tree->root->ne->body == body2);
  assert(tree->root->ne->x == 0.75);
  assert(tree->root->ne->y == 0.75);
  assert(tree->root->ne->mass == 1.0);
  quadtree_free(tree);
}

int
main(int argc, const char *argv[]) {
  test(quad);
  test(node);
  test(distance_between_bodies);
  test(update_center_of_mass);
  test(insert_one_body);
  test(insert_two_bodies);
}
