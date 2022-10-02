
#pragma once

#include "util.hpp"

#include "hittable.hpp"
#include "sphere.hpp"

struct animated_sphere : public hittable {
  point3 centre0 = point3(0.0), centre1 = point3(0.0);
  real t0 = 0.0, t1 = 1.0;
  real radius = 1.0;
  material *mat_ptr = nullptr;

  constexpr animated_sphere() = default;
  constexpr animated_sphere(const point3 &centre0, const point3 &centre1,
                            const real t0, const real t1, const real radius,
                            material *mat_ptr)
      : centre0(centre0), centre1(centre1), t0(t0), t1(t1), radius(radius),
        mat_ptr(mat_ptr) {}
  virtual ~animated_sphere() = default;

  virtual bool hit(const ray &r, const real t_min, const real t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(const real time0, const real time1,
                            aabb &output_box) const override;

  constexpr inline point3 get_centre(const real time) const {
    const real t = (time - t0) / (t1 - t0);
    return (1 - t) * centre0 + t * centre1;
  }
};
