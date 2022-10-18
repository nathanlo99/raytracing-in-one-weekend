
#pragma once

#include "aabb.hpp"
#include "hittable.hpp"
#include "material.hpp"
#include "triangle.hpp"

#include <optional>

struct quad : public hittable {
  const vertex m_p0, m_p1, m_p2;
  material *m_mat_ptr;
  aabb m_bounding_box;

  constexpr quad(const vertex &p0, const vertex &p1, const vertex &p2,
                 material *mat)
      : m_p0(p0), m_p1(p1), m_p2(p2), m_mat_ptr(mat) {
    const point3 p3 = p1.position + p2.position - p0.position;
    m_bounding_box.merge(p0.position);
    m_bounding_box.merge(p1.position);
    m_bounding_box.merge(p2.position);
    m_bounding_box.merge(p3);
    m_bounding_box.expand(eps);
  }

  virtual ~quad() {}

  virtual bool hit(const ray &r, const real t_min, const real t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(const real time0, const real time1,
                            aabb &output_box) const override;
};
