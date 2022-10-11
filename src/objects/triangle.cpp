
#include "triangle.hpp"

__attribute__((hot)) bool triangle::hit(const ray &r, const real t_min,
                                        const real t_max,
                                        hit_record &rec) const {
  const vec3 edge1 = m_p1.position - m_p0.position;
  const vec3 edge2 = m_p2.position - m_p0.position;
  const vec3 rop0 = r.orig - m_p0.position;
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

  // Compute uv's
  const vec3 uv0 = m_p0.uv.value_or(vec3(0.0, 0.0, 0.0));
  const vec3 uv1 = m_p1.uv.value_or(vec3(1.0, 0.0, 0.0));
  const vec3 uv2 = m_p2.uv.value_or(vec3(0.0, 1.0, 0.0));
  const vec3 uv = uv0 + u * (uv1 - uv0) + v * (uv2 - uv0);

  rec.u = uv[0];
  rec.v = uv[1];

  const vec3 normal0 = m_p0.normal.value_or(n);
  const vec3 normal1 = m_p1.normal.value_or(n);
  const vec3 normal2 = m_p2.normal.value_or(n);
  const vec3 normal =
      normal0 + u * (normal1 - normal0) + v * (normal2 - normal0);
  rec.set_face_normal(r, normal);

  rec.mat_ptr = m_mat_ptr;
  return true;
}

bool triangle::bounding_box(const real time0, const real time1,
                            aabb &output_box) const {
  output_box = m_bounding_box;
  return true;
}
