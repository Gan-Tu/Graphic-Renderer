#include "sphere.h"

#include <cmath>

#include  "../bsdf.h"
#include "../misc/sphere_drawing.h"

namespace CGL { namespace StaticScene {

bool Sphere::test(const Ray& r, double& t1, double& t2) const {

  // Implement ray - sphere intersection test.
  // Return true if there are intersections and writing the
  // smaller of the two intersection times in t1 and the larger in t2.
  double a = r.d.norm2();
  double b = 2 * dot(r.o - o, r.d);
  double c = (r.o - o).norm2() - r2;
  double delta = b*b - 4*a*c;
  if (delta < 0) {
    return false;
  }
  t1 = (-b - sqrt(delta)) / (2*a);
  t2 = (-b + sqrt(delta)) / (2*a);
  if (t1 < 0 || t1 < r.min_t || t1 > r.max_t) {
    if (t2 < 0 || t2 < r.min_t || t2 > r.max_t) {
      return false;
    } else {
      r.max_t = t2;
    }
  } else {
    r.max_t = t1;
  }

  return true;
}

bool Sphere::intersect(const Ray& r) const {

  // Implement ray - sphere intersection.
  // Note that you might want to use the the Sphere::test helper here.
  double t1, t2;
  return test(r, t1, t2);

}

bool Sphere::intersect(const Ray& r, Intersection *i) const {

  // Implement ray - sphere intersection.
  // Note again that you might want to use the the Sphere::test helper here.
  // When an intersection takes place, the Intersection data should be updated
  // correspondingly.
  double t1, t2;
  if (!test(r, t1, t2)) {
    return false;
  }
  double t = t1;
  if (t1 < 0) {
    t = t2;
  }

  i->t = r.max_t;
  i->n = normal(r.o + t * r.d);
  i->primitive = this;
  i->bsdf = get_bsdf();

  return true;
}

void Sphere::draw(const Color& c) const {
  Misc::draw_sphere_opengl(o, r, c);
}

void Sphere::drawOutline(const Color& c) const {
    //Misc::draw_sphere_opengl(o, r, c);
}


} // namespace StaticScene
} // namespace CGL
