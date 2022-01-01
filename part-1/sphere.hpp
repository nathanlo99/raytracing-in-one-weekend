
#pragma once

#include "hittable.hpp"
#include "vec3.hpp"

struct sphere : public hittable {
  point3 centre;
  double radius;
  shared_ptr<material> mat_ptr;

  sphere() = default;
  sphere(const point3 &centre, double r, shared_ptr<material> material)
      : centre(centre), radius(r), mat_ptr(material) {}

  virtual bool hit(const ray &r, double t_min, double t_max,
                   hit_record &rec) const override {
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
};
