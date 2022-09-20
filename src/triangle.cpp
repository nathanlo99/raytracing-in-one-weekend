
#include "triangle.hpp"

#define OLD_TRIANGLE_HIT 0

__attribute__((hot)) bool triangle::hit(const ray &r, const float t_min,
                                        const float t_max,
                                        hit_record &rec) const {
#if OLD_TRIANGLE_HIT
  // NOTE: This is almost directly transcribed from
  // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

  // TODO: Can we do more optimizations using t_min and t_max?
  const vec3 edge1 = p1 - p0;
  const vec3 edge2 = p2 - p0;
  const vec3 h = cross(r.dir, edge2);
  const float a = dot(edge1, h);
  if (a > -eps && a < eps)
    return false;
  const float f = 1.0 / a;
  const vec3 s = r.orig - p0;
  const float u = f * dot(s, h);
  if (u < 0.0 || u > 1.0)
    return false;
  const vec3 q = cross(s, edge1);
  const float v = f * dot(r.dir, q);
  if (v < 0.0 || u + v > 1.0)
    return false;

  const float t = f * dot(edge2, q);
  if (t < t_min || t > t_max)
    return false;

  rec.t = t;
  rec.p = r.at(t);
  rec.set_face_normal(r, normal);
  // TODO: fix uv's for cases where the triangle comes with uv's
  rec.u = u;
  rec.v = v;
  rec.mat_ptr = mat_ptr;
  return true;

#else

  const vec3 edge1 = p1 - p0;
  const vec3 edge2 = p2 - p0;
  const vec3 rop0 = r.orig - p0;
  const vec3 n = cross(edge1, edge2);
  const float a = dot(r.dir, n);
  if (a > -eps && a < eps)
    return false;
  const float d = 1.0 / a;
  const float t = -d * dot(n, rop0);
  if (t < t_min || t > t_max)
    return false;

  const vec3 q = cross(rop0, r.dir);
  const float u = -d * dot(q, edge2);
  if (u < 0.0 || u > 1.0)
    return false;
  const float v = d * dot(q, edge1);
  if (v < 0.0 || u + v > 1.0)
    return false;

  rec.t = t;
  rec.p = r.at(t);
  rec.set_face_normal(r, normal);
  // TODO: fix uv's for cases where the triangle comes with uv's
  rec.u = u;
  rec.v = v;
  rec.mat_ptr = mat_ptr;
  return true;
#endif
}

bool triangle::bounding_box(aabb &output_box) const {
  output_box.min = min(p0, min(p1, p2)) - vec3(eps);
  output_box.max = max(p0, max(p1, p2)) + vec3(eps);
  return true;
}
