
#include "sphere.hpp"

__attribute__((hot)) bool sphere::hit(const ray &r, const float t_min,
                                      const float t_max,
                                      hit_record &rec) const {
  const vec3 oc = r.orig - centre;
  const float a = dot(r.dir, r.dir);
  const float half_b = dot(oc, r.dir);
  const float c = dot(oc, oc) - radius * radius;

  const float discriminant = half_b * half_b - a * c;
  if (discriminant < 0)
    return false;
  const float sqrtd = std::sqrt(discriminant);

  // Find the nearest root that lies in the acceptable range.
  float root = (-half_b - sqrtd) / a;
  if (root < t_min || root > t_max) {
    root = (-half_b + sqrtd) / a;
    if (root < t_min || root > t_max)
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

bool sphere::bounding_box(const float time0, const float time1,
                          aabb &output_box) const {
  output_box = aabb(centre - vec3(radius), centre + vec3(radius));
  return true;
}
