
#include "triangle.hpp"

__attribute__((hot)) bool triangle::hit(const ray &r, const double t_min,
                                        const double t_max,
                                        hit_record &rec) const {
  // NOTE: This is almost directly transcribed from
  // https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm

  // TODO: Can we do more optimizations using t_min and t_max?
  const vec3 edge1 = p2 - p1;
  const vec3 edge2 = p3 - p1;
  const vec3 h = cross(r.dir, edge2);
  const float a = dot(edge1, h);
  if (a > -eps && a < eps)
    return false;
  const float f = 1.0 / a;
  const vec3 s = r.orig - p1;
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
}

bool triangle::bounding_box(const double time0, const double time1,
                            aabb &output_box) const {
  output_box.min = min(p1, min(p2, p3)) - vec3(eps);
  output_box.max = max(p1, max(p2, p3)) + vec3(eps);
  return true;
}
