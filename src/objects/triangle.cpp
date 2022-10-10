
#include "triangle.hpp"

bool triangle::hit(const ray &r, const real t_min, const real t_max,
                   hit_record &rec) const {
  const vec3 edge1 = m_p1 - m_p0;
  const vec3 edge2 = m_p2 - m_p0;
  const vec3 rop0 = r.orig - m_p0;
  const vec3 n = glm::cross(edge1, edge2);
  const real a = glm::dot(r.dir, n);
  // if (a > -eps && a < eps)
  //   return false;
  const real d = 1.0 / a;
  const real t = -d * glm::dot(n, rop0);
  if (t < t_min || t > t_max)
    return false;

  const vec3 q = glm::cross(rop0, r.dir);
  const real u = -d * glm::dot(q, edge2);
  if (u < 0.0 || u > 1.0)
    return false;
  const real v = d * glm::dot(q, edge1);
  if (v < 0.0 || u + v > 1.0)
    return false;

  rec.t = t;
  rec.p = r.at(t);
  rec.set_face_normal(r, n);
  // TODO: fix uv's for cases where the triangle comes with uv's
  rec.u = u;
  rec.v = v;
  rec.mat_ptr = m_mat_ptr;
  return true;
}

bool triangle::bounding_box(const real time0, const real time1,
                            aabb &output_box) const {
  output_box.min = glm::min(m_p0, glm::min(m_p1, m_p2)) - vec3(eps);
  output_box.max = glm::max(m_p0, glm::max(m_p1, m_p2)) + vec3(eps);
  return true;
}
