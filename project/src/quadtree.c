#include <stdio.h>
#include <omp.h>
#include "quadtree.h"

/* Internal prototypes */
int
insert_(quadtree_node_t *node, quadtree_body_t *body);

void
update_force_(quadtree_node_t *node, quadtree_body_t *body, double g, double threshold);

int
count_approximations(quadtree_node_t *node, quadtree_body_t body, double threshold);

/*****************************************************************************/
quadtree_body_t*
quadtree_body_new(double p_x, double p_y,
                  double v_x, double v_y,
                  double f_x, double f_y,
                  double mass)
{
  quadtree_body_t *body;
  if(!(body = malloc(sizeof(*body)))) {
    return NULL;
  }
  body->p_x = p_x;
  body->p_y = p_y;
  body->v_x = v_x;
  body->v_y = v_y;
  body->f_x = f_x;
  body->f_y = f_y;
  body->mass = mass;

  return body;
}
/*****************************************************************************/
void
quadtree_body_free(quadtree_body_t *body) {
  free(body);
}
/*****************************************************************************/
quadtree_quad_t*
quadtree_quad_new(double x, double y, double length) {
  quadtree_quad_t *quad;
  if(!(quad = malloc(sizeof(*quad)))) {
    return NULL;
  }
  quad->x = x;
  quad->y = y;
  quad->length = length;
  return  quad;
}
/*****************************************************************************/
void
quadtree_quad_free(quadtree_quad_t *quad) {
  free(quad);
}
/*****************************************************************************/
quadtree_node_t*
quadtree_node_new() {
  quadtree_node_t *node;
  if(!(node = malloc(sizeof(*node)))) {
    return NULL;
  }
  node->nw     = NULL;
  node->ne     = NULL;
  node->sw     = NULL;
  node->se     = NULL;
  node->quad   = NULL;
  node->body   = NULL;

  return node;
}
/*****************************************************************************/
void
quadtree_node_free(quadtree_node_t *node) {
  if(node->nw != NULL) quadtree_node_free(node->nw);
  if(node->ne != NULL) quadtree_node_free(node->ne);
  if(node->sw != NULL) quadtree_node_free(node->sw);
  if(node->se != NULL) quadtree_node_free(node->se);

  quadtree_quad_free(node->quad);
  // quadtree_body_free(node->body);
  free(node);
}
/*****************************************************************************/
int
node_is_empty(quadtree_node_t *node) {
  return (node->body == NULL) &&
          (node->nw == NULL) &&
          (node->ne == NULL) &&
          (node->sw == NULL) &&
          (node->se == NULL);
}
/*****************************************************************************/
int
node_is_leaf(quadtree_node_t *node) {
  return (node->body != NULL) &&
          (node->nw == NULL) &&
          (node->ne == NULL) &&
          (node->sw == NULL) &&
          (node->se == NULL);
}
/*****************************************************************************/
quadtree_t*
quadtree_new(double length, double threshold) {
  quadtree_t *tree;
  if(!(tree = malloc(sizeof(*tree)))) {
    return NULL;
  }
  if(!(tree->root = quadtree_node_new())){
    return NULL;
  }
  if(!(tree->root->quad = quadtree_quad_new(0.0, 0.0, length))) {
    return NULL;
  }
  tree->threshold = threshold;

  return tree;
}
/*****************************************************************************/
void
quadtree_free(quadtree_t *tree) {
  quadtree_node_free(tree->root);
  free(tree);
}
/*****************************************************************************/
double
distance_between_bodies(quadtree_body_t *a, quadtree_body_t *b) {
  return sqrt(pow(a->p_x - b->p_x, 2) + pow(a->p_y - b->p_y, 2));
}
/*****************************************************************************/
double
distance_between_points(double x1, double y1, double x2, double y2) {
  return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}
/*****************************************************************************/
void
update_center_of_mass(quadtree_node_t *node, quadtree_body_t *body) {
  double mass = node->mass + body->mass;
  node->p_x = (node->p_x * node->mass + body->p_x * body->mass) / mass;
  node->p_y = (node->p_y * node->mass + body->p_y * body->mass) / mass;
  node->mass = mass;
}
/*****************************************************************************/
quadtree_node_t *
node_with_quad(double x, double y, double length) {
  quadtree_node_t *node;

  if(!(node = quadtree_node_new())) return NULL;
  if(!(node->quad = quadtree_quad_new(x, y, length))) return NULL;

  return node;
}
/*****************************************************************************/
int
insert_body(quadtree_t *tree, quadtree_body_t *body) {
  return insert_(tree->root, body);
}
/*****************************************************************************/
// TODO Make this inline?
int
quad_contains(quadtree_quad_t *quad, quadtree_body_t *body) {
  return quad != NULL &&
          body->p_x >= quad->x &&
          body->p_x <= (quad->x + quad->length) &&
          body->p_y >= quad->y &&
          body->p_y <= (quad->y + quad->length);
}
/*****************************************************************************/
quadtree_node_t *
get_quad(quadtree_node_t *node, quadtree_body_t *body) {
  if(node->nw == NULL) {
    return NULL;
  }
  if(quad_contains(node->nw->quad, body)) return node->nw;
  if(quad_contains(node->ne->quad, body)) return node->ne;
  if(quad_contains(node->sw->quad, body)) return node->sw;
  if(quad_contains(node->se->quad, body)) return node->se;
  return NULL;
}
/*****************************************************************************/
int
split_(quadtree_node_t *node) {
  quadtree_node_t *nw;
  quadtree_node_t *ne;
  quadtree_node_t *sw;
  quadtree_node_t *se;

  double length = node->quad->length / 2;

  if(!(nw = node_with_quad(node->quad->x, node->quad->y+length, length))) return 0;
  if(!(ne = node_with_quad(node->quad->x+length, node->quad->y+length, length))) return 0;
  if(!(sw = node_with_quad(node->quad->x, node->quad->y, length))) return 0;
  if(!(se = node_with_quad(node->quad->x+length, node->quad->y, length))) return 0;

  node->nw = nw;
  node->ne = ne;
  node->sw = sw;
  node->se = se;
  return 1;
}
/*****************************************************************************/
int
insert_(quadtree_node_t *node, quadtree_body_t *body) {
  quadtree_node_t *quadrant;

  if(!quad_contains(node->quad, body)) {
    // Body is outside the bounding box
    return 0;
  }

  // TODO What happens if the node contains a body at the same location?

  // Is the node external and without body, i.e. empty?
  if(node_is_empty(node)) {
    node->body = body;
    node->p_x = body->p_x;
    node->p_y = body->p_y;
    node->v_x = body->v_x;
    node->v_y = body->v_y;
    node->f_x = body->f_x;
    node->f_y = body->f_y;
    node->mass = body->mass;
    return 1;
  }
  // If not, is it internal?
  else if((quadrant = get_quad(node, body)) != NULL) {
    update_center_of_mass(node, body);
    return insert_(quadrant, body);
  }
  // Then it must be external but containing a body
  else {
    if(!split_(node)) {
      // Failed to split node
      return 0;
    }
    quadrant = get_quad(node, body);
    insert_(quadrant, body);
    quadrant = get_quad(node, node->body);
    insert_(quadrant, node->body);
    update_center_of_mass(node, body);
    node->body = NULL;
    return 1;
  }
}
/*****************************************************************************/
void
update_force(quadtree_t *tree, quadtree_body_t *body, double g) {
  return update_force_(tree->root, body, g, tree->threshold);
}
/*****************************************************************************/
void
update_force_(quadtree_node_t *node, quadtree_body_t *body,
                double g, double threshold)
{
  double distance;
  double magnitude;
  double dir_x;
  double dir_y;

  if(node_is_empty(node)) {
    return;
  } else if(node_is_leaf(node)) {
    // Check that the node isn't the same as body
    if(node->body == body) {
      return;
    }
    distance = distance_between_points(node->p_x, node->p_y, body->p_x, body->p_y);
    magnitude = (g*node->mass*body->mass) / (distance*distance);
    dir_x = node->p_x - body->p_x;
    dir_y = node->p_y - body->p_y;
    body->f_x = body->f_x + magnitude * dir_x / distance;
    body->f_y = body->f_y + magnitude * dir_y / distance;
    return;
  } else {
      distance = distance_between_points(node->p_x, node->p_y, body->p_x, body->p_y);
      if(distance > threshold) {
        // sufficiently far away to make approximation
        magnitude = (g*node->mass*body->mass) / distance*distance;
        dir_x = node->p_x - body->p_x;
        dir_y = node->p_y - body->p_y;
        body->f_x = body->f_x + magnitude * dir_x / distance;
        body->f_y = body->f_y + magnitude * dir_y / distance;
        return;
      } else {
        // Too close, run recursively on each child node
        update_force_(node->nw, body, g, threshold);
        update_force_(node->ne, body, g, threshold);
        update_force_(node->sw, body, g, threshold);
        update_force_(node->se, body, g, threshold);
        return;
      }
  }
}
/*****************************************************************************/
void
move_bodies(quadtree_body_t *bodies, int n, double dt) {
  double deltav_x, deltav_y;
  double deltap_x, deltap_y;
  for (int i = 0; i < n; i++) {
    deltav_x = bodies[i].f_x/bodies[i].mass * dt;
    deltav_y = bodies[i].f_y/bodies[i].mass * dt;
    deltap_x = (bodies[i].v_x + deltav_x/2) * dt;
    deltap_y = (bodies[i].v_y + deltav_y/2) * dt;
    bodies[i].v_x = bodies[i].v_x + deltav_x;
    bodies[i].v_y = bodies[i].v_y + deltav_y;
    bodies[i].p_x = bodies[i].p_x + deltap_x;
    bodies[i].p_y = bodies[i].p_y + deltap_y;
    bodies[i].f_x = 0.0;
    bodies[i].f_y = 0.0;
  }
}
/*****************************************************************************/
int
test_far_value(quadtree_t *tree, int n, quadtree_body_t *bodies) {
  double approximation_ratio = 1;
  int number_of_approximations = 0;
  for(int i = 0; i < n; i++) {
    number_of_approximations = count_approximations(tree->root, bodies[i], tree->threshold);
    approximation_ratio += (double)(number_of_approximations) / (double)(n - 1);
    // printf("Body %d Appr %d Ratio %f\n", i+1, number_of_approximations, (double)(n - number_of_approximations) / (double)(n - 1));
  }
  // printf("approximation_ratio %f\n", approximation_ratio/n);
  if(approximation_ratio/n >= 0.8) {
    return 1;
  }
  return 0;
}
/*****************************************************************************/
/**
  * Counts the number of nodes that are so far away form the body that an
  * approximation can be used
  */
int
count_approximations(quadtree_node_t *node, quadtree_body_t body, double threshold) {
  double distance;

  if(node_is_empty(node)) {
    return 0;
  } else if(node_is_leaf(node)) {
    distance = distance_between_points(node->p_x, node->p_y, body.p_x, body.p_y);
    if(distance > threshold) {
      return 1;
    } else {
      return 0;
    }
  } else {
      int nw = count_approximations(node->nw, body, threshold);
      int ne = count_approximations(node->ne, body, threshold);
      int sw = count_approximations(node->sw, body, threshold);
      int se = count_approximations(node->se, body, threshold);
      return nw + ne + sw + se;
  }
}
/*****************************************************************************/
