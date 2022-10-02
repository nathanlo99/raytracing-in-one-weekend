
#pragma once

#include "aabb.hpp"
#include "hittable.hpp"

struct sphere : public hittable {
  point3 centre = point3(0.0);
  real radius = 1.0;
  material *mat_ptr = nullptr;

  constexpr sphere() = default;
  constexpr sphere(const point3 &centre, const real r, material *mat)
      : centre(centre), radius(r), mat_ptr(mat) {}
  virtual ~sphere() {}

  virtual bool hit(const ray &r, const real t_min, const real t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(const real time0, const real time1,
                            aabb &output_box) const override;

  static constexpr void get_sphere_uv(const point3 &p, real &u, real &v) {
    const real theta = acos(-p.y);
    const real phi = atan2(-p.z, p.x) + pi;
    u = phi / (2.0 * pi);
    v = theta / pi;
  }
};
