
#pragma once

#include "aabb.hpp"
#include "hittable.hpp"
#include "material.hpp"

struct triangle : public hittable {
  point3 p0, p1, p2;
  vec3 normal;
  material *mat_ptr;

  triangle(const point3 &p0, const point3 &p1, const point3 &p2, material *mat)
      : p0(p0), p1(p1), p2(p2), normal(normalize(cross(p1 - p0, p2 - p0))),
        mat_ptr(mat) {}

  virtual ~triangle() {}

  virtual bool hit(const ray &r, const float t_min, const float t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(aabb &output_box) const override;
};
