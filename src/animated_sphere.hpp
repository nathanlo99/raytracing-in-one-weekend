
#pragma once

#include "util.hpp"

#include "hittable.hpp"

struct animated_sphere : public hittable {
  point3 centre0, centre1;
  double t0, t1;
  double radius;
  shared_ptr<material> mat_ptr;

  animated_sphere(const point3 &centre0, const point3 &centre1, const double t0,
                  const double t1, const double radius,
                  shared_ptr<material> mat_ptr)
      : centre0(centre0), centre1(centre1), t0(t0), t1(t1), radius(radius),
        mat_ptr(mat_ptr) {}
  virtual ~animated_sphere() = default;

  virtual bool hit(const ray &r, const double t_min, const double t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(const double time0, const double time1,
                            aabb &output_box) const override;

  constexpr inline point3 get_centre(const double time) const {
    const double t = (time - t0) / (t1 - t0);
    return (1 - t) * centre0 + t * centre1;
  }
};

bool animated_sphere::hit(const ray &r, const double t_min, const double t_max,
                          hit_record &rec) const {
  const point3 centre = get_centre(r.time);
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
  rec.mat_ptr = mat_ptr;

  return true;
}

bool animated_sphere::bounding_box(const double time0, const double time1,
                                   aabb &output_box) const {
  const aabb box0(centre0 - vec3(radius), centre0 + vec3(radius));
  const aabb box1(centre1 - vec3(radius), centre1 + vec3(radius));
  output_box = surrounding_box(box0, box1);
  return true;
}
