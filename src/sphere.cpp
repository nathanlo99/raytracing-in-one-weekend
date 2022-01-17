
#include "sphere.hpp"

__attribute__((hot)) bool sphere::hit(const ray &r, const double t_min,
                                      const double t_max,
                                      hit_record &rec) const {
  const vec3 oc = r.orig - centre;
  const double a = r.dir.length_squared();
  const double half_b = dot(oc, r.dir);
  const double c = oc.length_squared() - radius * radius;

  const double discriminant = half_b * half_b - a * c;
  if (discriminant < 0)
    return false;
  const double sqrtd = std::sqrt(discriminant);

  // Find the nearest root that lies in the acceptable range.
  double root = (-half_b - sqrtd) / a;
  if (root < t_min || t_max < root) {
    root = (-half_b + sqrtd) / a;
    if (root < t_min || t_max < root)
      return false;
  }

  rec.t = root;
  rec.p = r.at(rec.t);
  const vec3 outward_normal = (rec.p - centre) / radius;
  rec.set_face_normal(r, outward_normal);
  get_sphere_uv(outward_normal, rec.u, rec.v);
  rec.mat_ptr = mat_ptr;

  return true;
}

bool sphere::bounding_box(const double time0, const double time1,
                          aabb &output_box) const {
  output_box = aabb(centre - vec3(radius), centre + vec3(radius));
  return true;
}
