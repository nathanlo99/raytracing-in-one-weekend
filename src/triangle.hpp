
#pragma once

#include "aabb.hpp"
#include "hittable.hpp"
#include "material.hpp"

struct triangle : public hittable {
  point3 p0, p1, p2;
  material *mat_ptr;

  constexpr triangle(const point3 &p0, const point3 &p1, const point3 &p2,
                     material *mat)
      : p0(p0), p1(p1), p2(p2), mat_ptr(mat) {}

  virtual ~triangle() {}

  virtual bool hit(const ray &r, const real t_min, const real t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(const real time0, const real time1,
                            aabb &output_box) const override;
};
