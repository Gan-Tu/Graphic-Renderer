#include "bvh.h"

#include "CGL/CGL.h"
#include "static_scene/triangle.h"

#include <iostream>
#include <stack>

using namespace std;

namespace CGL { namespace StaticScene {

BVHAccel::BVHAccel(const std::vector<Primitive *> &_primitives,
                   size_t max_leaf_size) {

  root = construct_bvh(_primitives, max_leaf_size);

}

BVHAccel::~BVHAccel() {
  if (root) delete root;
}

BBox BVHAccel::get_bbox() const {
  return root->bb;
}

void BVHAccel::draw(BVHNode *node, const Color& c) const {
  if (node->isLeaf()) {
    for (Primitive *p : *(node->prims))
      p->draw(c);
  } else {
    draw(node->l, c);
    draw(node->r, c);
  }
}

void BVHAccel::drawOutline(BVHNode *node, const Color& c) const {
  if (node->isLeaf()) {
    for (Primitive *p : *(node->prims))
      p->drawOutline(c);
  } else {
    drawOutline(node->l, c);
    drawOutline(node->r, c);
  }
}

BVHNode *BVHAccel::construct_bvh(const std::vector<Primitive*>& prims, size_t max_leaf_size) {
  
  // Construct a BVH from the given vector of primitives and maximum leaf
  // size configuration. The starter code build a BVH aggregate with a
  // single leaf node (which is also the root) that encloses all the
  // primitives.


  // calculate bounding box of all primitives
  BBox centroid_box, bbox;
  for (Primitive *p : prims) {
    BBox bb = p->get_bbox();
    bbox.expand(bb);
    Vector3D c = bb.centroid();
    centroid_box.expand(c);
  }
  BVHNode *node = new BVHNode(bbox);


  // base case: leaf node
  if (prims.size() <= max_leaf_size) {
    node->prims = new vector<Primitive *>(prims);
    return node;
  }

  // pick the largest dimension of the bounding box's extent as axis to split on
  int axis = -1; // NULL: -1, x: 0, y: 1, z: 2
  if (bbox.extent.x >= bbox.extent.y &&  bbox.extent.x >= bbox.extent.z) {
    axis = 0;
  } else if (bbox.extent.y >= bbox.extent.x &&  bbox.extent.y >= bbox.extent.z) {
    axis = 1;
  } else {
    axis = 2;
  }

  // calculate the midpoint of the bounding box as the split point
  double left_edge = bbox.min[axis], right_edge = bbox.max[axis];
  double split_midpoint;

  // split all primitives into two new vectors based on their bounding box centroids
  vector<Primitive *> *left_p = new vector<Primitive *>();
  vector<Primitive *> *right_p = new vector<Primitive *>();

  // we keep trying to split until we have left and right child both nonempty
  // if one axis fails, we try other axis
  int fail_count = 0, FAIL_MAX_ATTEMPT = 3; 

  while (left_p->empty() || right_p->empty()) {
    split_midpoint = (right_edge + left_edge) / 2;
    left_p->clear();
    right_p->clear();
    for (Primitive *p : prims) {
      if (p->get_bbox().centroid()[axis] <= split_midpoint) {
        left_p->push_back(p);
      } else {
        right_p->push_back(p);
      }
    }
    // recalculate midpoint if one side is empty
    if (left_p->empty()) {
      left_edge = split_midpoint;
      fail_count++;
    } else if (right_p->empty()) {
      right_edge = split_midpoint;
      fail_count++;
    }
    if (left_edge >= right_edge || fail_count >= FAIL_MAX_ATTEMPT) {
      axis = (axis + 1) % 3; // try next axis, looping between 0..2
      fail_count = 0;
      left_edge = bbox.min[axis];
      right_edge = bbox.max[axis];
    }
  }

  // recurse
  node->l = construct_bvh(*left_p,  max_leaf_size);
  node->r = construct_bvh(*right_p, max_leaf_size);

  return node;
}


bool BVHAccel::intersect(const Ray& ray, BVHNode *node) const {
  // Fill in the intersect function.
  // Take note that this function has a short-circuit that the
  // Intersection version cannot, since it returns as soon as it finds
  // a hit, it doesn't actually have to find the closest hit.

  // ray misses node.bb
  double t0, t1;
  if (!node->bb.intersect(ray, t0, t1)) {
    return false;
  }
  if (t1 < ray.min_t || t0 > ray.max_t) {
    return false;
  }

  // node is a leaf node
  if (node->isLeaf()) {
    for (Primitive *p : *(node->prims)) {
      total_isects++;
      if (p->intersect(ray)) 
        return true;
    }
  }

  return intersect(ray, node->l) || intersect(ray, node->r);
}

bool BVHAccel::intersect(const Ray& ray, Intersection* i, BVHNode *node) const {
  // Fill in the intersect function.

  // ray misses node.bb
  double t0, t1;
  if (!node->bb.intersect(ray, t0, t1)) {
    return false;
  }
  if (t1 < ray.min_t || t0 > ray.max_t) {
    return false;
  }

  // node is a leaf node
  if (node->isLeaf()) {
    bool hit = false;
    for (Primitive *p : *(node->prims)) {
      total_isects++;
      if (p->intersect(ray, i)) 
        hit = true;
    }
    return hit;
  }

  bool hit = intersect(ray, i, node->l);
  bool hit2 = intersect(ray, i, node->r);

  return hit || hit2;
  
}

}  // namespace StaticScene
}  // namespace CGL
