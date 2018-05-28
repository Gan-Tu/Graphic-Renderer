#include "bbox.h"

#include "GL/glew.h"

#include <algorithm>
#include <iostream>

namespace CGL {

bool BBox::intersect(const Ray& r, double& t0, double& t1) const {

  // Implement ray - bounding box intersection test
  // If the ray intersected the bouding box within the range given by
  // t0, t1, update t0 and t1 with the new intersection times.
  Vector3D t_min_p_minus_o = min - r.o;
  Vector3D t_max_p_minus_o = max - r.o;

  double t_xmin = std::min(t_min_p_minus_o.x / r.d.x, t_max_p_minus_o.x / r.d.x);
  double t_xmax = std::max(t_min_p_minus_o.x / r.d.x, t_max_p_minus_o.x / r.d.x);

  double t_ymin = std::min(t_min_p_minus_o.y / r.d.y, t_max_p_minus_o.y / r.d.y);
  double t_ymax = std::max(t_min_p_minus_o.y / r.d.y, t_max_p_minus_o.y / r.d.y);

  double t_zmin = std::min(t_min_p_minus_o.z / r.d.z, t_max_p_minus_o.z / r.d.z);
  double t_zmax = std::max(t_min_p_minus_o.z / r.d.z, t_max_p_minus_o.z / r.d.z);

  if (t_ymax < t_xmin || t_xmax < t_ymin) {
    return false;
  }

  // ensure non-destructiveness of intersect function
  double tmp0, tmp1;
  tmp0 = std::max(t_xmin, t_ymin);
  tmp1 = std::min(t_xmax, t_ymax);

  if (tmp1 < t_zmin || tmp0 > t_zmax) {
    return false;
  }
  t0 = std::max(tmp0, t_zmin);
  t1 = std::min(tmp1, t_zmax);

  return true;

}

void BBox::draw(Color c) const {

  glColor4f(c.r, c.g, c.b, c.a);

	// top
	glBegin(GL_LINE_STRIP);
	glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, max.y, min.z);
  glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, max.y, max.z);
  glVertex3d(max.x, max.y, max.z);
	glEnd();

	// bottom
	glBegin(GL_LINE_STRIP);
  glVertex3d(min.x, min.y, min.z);
  glVertex3d(min.x, min.y, max.z);
  glVertex3d(max.x, min.y, max.z);
  glVertex3d(max.x, min.y, min.z);
  glVertex3d(min.x, min.y, min.z);
	glEnd();

	// side
	glBegin(GL_LINES);
	glVertex3d(max.x, max.y, max.z);
  glVertex3d(max.x, min.y, max.z);
	glVertex3d(max.x, max.y, min.z);
  glVertex3d(max.x, min.y, min.z);
	glVertex3d(min.x, max.y, min.z);
  glVertex3d(min.x, min.y, min.z);
	glVertex3d(min.x, max.y, max.z);
  glVertex3d(min.x, min.y, max.z);
	glEnd();

}

std::ostream& operator<<(std::ostream& os, const BBox& b) {
  return os << "BBOX(" << b.min << ", " << b.max << ")";
}

} // namespace CGL
