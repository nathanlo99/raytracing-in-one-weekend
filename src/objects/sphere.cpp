
#include "sphere.hpp"

__attribute__((hot)) bool sphere::hit(const ray &r, const real t_min,
                                      const real t_max, hit_record &rec) const {
  const vec3 oc = r.orig - m_centre;
  const real a = dot(r.dir, r.dir);
  const real half_b = dot(oc, r.dir);
  const real c = dot(oc, oc) - m_radius * m_radius;

  const real discriminant = half_b * half_b - a * c;
  if (discriminant < 0)
    return false;
  const real sqrtd = std::sqrt(discriminant);

  // Find the nearest root that lies in the acceptable range.
  real root = (-half_b - sqrtd) / a;
  if (root < t_min || root > t_max) {
    root = (-half_b + sqrtd) / a;
    if (root < t_min || root > t_max)
      return false;
  }

  rec.t = root;
  rec.p = r.at(rec.t);
  const vec3 outward_normal = (rec.p - m_centre) / m_radius;
  rec.set_face_normal(r, outward_normal);
  get_sphere_uv(outward_normal, rec.u, rec.v);
  rec.mat_ptr = m_mat_ptr;

  return true;
}

bool sphere::bounding_box(const real time0, const real time1,
                          aabb &output_box) const {
  output_box = aabb(m_centre - vec3(m_radius), m_centre + vec3(m_radius));
  return true;
}
