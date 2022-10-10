
#include "animated_sphere.hpp"

__attribute__((hot)) bool animated_sphere::hit(const ray &r, const real t_min,
                                               const real t_max,
                                               hit_record &rec) const {
  const point3 centre = get_centre(r.time);
  const vec3 oc = r.orig - centre;
  const real a = glm::dot(r.dir, r.dir);
  const real half_b = glm::dot(oc, r.dir);
  const real c = glm::dot(oc, oc) - m_radius * m_radius;

  const real discriminant = half_b * half_b - a * c;
  if (discriminant < 0.0)
    return false;
  const real sqrtd = std::sqrt(discriminant);

  // Find the nearest root that lies in the acceptable range.
  real root = (-half_b - sqrtd) / a;
  if (root < t_min || t_max < root) {
    root = (-half_b + sqrtd) / a;
    if (root < t_min || t_max < root)
      return false;
  }

  rec.t = root;
  rec.p = r.at(rec.t);
  const vec3 outward_normal = (rec.p - centre) / m_radius;
  rec.set_face_normal(r, outward_normal);
  sphere::get_sphere_uv(outward_normal, rec.u, rec.v);
  rec.mat_ptr = m_mat_ptr;

  return true;
}

inline bool animated_sphere::bounding_box(const real time0, const real time1,
                                          aabb &output_box) const {
  output_box = aabb(glm::min(m_centre0, m_centre1) - vec3(m_radius),
                    glm::max(m_centre0, m_centre1) + vec3(m_radius));
  return true;
}