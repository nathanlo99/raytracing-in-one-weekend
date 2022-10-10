
#pragma once

#include "aabb.hpp"
#include "hittable.hpp"
#include "material.hpp"

struct triangle : public hittable {
  point3 m_p0, m_p1, m_p2;
  material *m_mat_ptr;

  constexpr triangle(const point3 &p0, const point3 &p1, const point3 &p2,
                     material *mat)
      : m_p0(p0), m_p1(p1), m_p2(p2), m_mat_ptr(mat) {}

  virtual ~triangle() {}

  virtual bool hit(const ray &r, const real t_min, const real t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(const real time0, const real time1,
                            aabb &output_box) const override;
};
