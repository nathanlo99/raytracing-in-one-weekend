
#pragma once

#include "aabb.hpp"
#include "hittable.hpp"
#include "vec3.hpp"

struct sphere : public hittable {
  point3 centre;
  double radius;
  material *mat_ptr;

  sphere() : centre(), radius(0), mat_ptr(nullptr) {}
  sphere(const point3 &centre, const double r, material *mat)
      : centre(centre), radius(r), mat_ptr(mat) {}
  virtual ~sphere() {}

  virtual bool hit(const ray &r, const double t_min, const double t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(const double time0, const double time1,
                            aabb &output_box) const override;

  static constexpr void get_sphere_uv(const point3 &p, double &u, double &v) {
    const double theta = acos(-p.y());
    const double phi = atan2(-p.z(), p.x()) + pi;
    u = phi / (2 * pi);
    v = theta / pi;
  }
};
