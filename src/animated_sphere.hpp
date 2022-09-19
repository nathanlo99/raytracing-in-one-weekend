
#pragma once

#include "util.hpp"

#include "hittable.hpp"
#include "sphere.hpp"

struct animated_sphere : public hittable {
  point3 centre0, centre1;
  float t0, t1;
  float radius;
  material *mat_ptr;

  animated_sphere(const point3 &centre0, const point3 &centre1, const float t0,
                  const float t1, const float radius, material *mat_ptr)
      : centre0(centre0), centre1(centre1), t0(t0), t1(t1), radius(radius),
        mat_ptr(mat_ptr) {}
  virtual ~animated_sphere() = default;

  virtual bool hit(const ray &r, const float t_min, const float t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(const float time0, const float time1,
                            aabb &output_box) const override;

  constexpr inline point3 get_centre(const float time) const {
    const float t = (time - t0) / (t1 - t0);
    return (1 - t) * centre0 + t * centre1;
  }
};

__attribute__((hot)) bool animated_sphere::hit(const ray &r, const float t_min,
                                               const float t_max,
                                               hit_record &rec) const {
  const point3 centre = get_centre(r.time);
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
  if (root < t_min || t_max < root) {
    root = (-half_b + sqrtd) / a;
    if (root < t_min || t_max < root)
      return false;
  }

  rec.t = root;
  rec.p = r.at(rec.t);
  const auto outward_normal = (rec.p - centre) / radius;
  rec.set_face_normal(r, outward_normal);
  sphere::get_sphere_uv(outward_normal, rec.u, rec.v);
  rec.mat_ptr = mat_ptr;

  return true;
}

bool animated_sphere::bounding_box(const float time0, const float time1,
                                   aabb &output_box) const {
  output_box = aabb(min(centre0, centre1) - vec3(radius),
                    max(centre0, centre1) + vec3(radius));
  return true;
}
