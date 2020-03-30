#include "quadtree.h"

/* Internal prototypes */
int
insert_(quadtree_node_t *node, quadtree_body_t *body);

/*****************************************************************************/
quadtree_body_t*
quadtree_body_new(double x, double y, double mass) {
  quadtree_body_t *body;
  if(!(body = malloc(sizeof(*body)))) {
    return NULL;
  }
  body->x = x;
  body->y = y;
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
  quadtree_body_free(node->body);
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
  return sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
}
/*****************************************************************************/
void
update_center_of_mass(quadtree_node_t *node, quadtree_body_t *body) {
  double mass = node->mass + body->mass;
  node->x = (node->x * node->mass + body->x * body->mass) / mass;
  node->y = (node->y * node->mass + body->y * body->mass) / mass;
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
          body->x >= quad->x &&
          body->x <= (quad->x + quad->length) &&
          body->y >= quad->y &&
          body->y <= (quad->y + quad->length);
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
  // TODO Have to make more thorough check if body is empty
  // Is the node external and without body, i.e. empty?
  if(node_is_empty(node)) {
    node->body = body;
    node->x = body->x;
    node->y = body->y;
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
