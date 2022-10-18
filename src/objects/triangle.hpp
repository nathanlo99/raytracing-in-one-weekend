
#pragma once

#include "aabb.hpp"
#include "hittable.hpp"
#include "material.hpp"

#include <optional>

struct vertex {
  const point3 position;
  const std::optional<vec3> uv;
  const std::optional<vec3> normal;

  constexpr vertex(const point3 &pos)
      : position(pos), uv(std::nullopt), normal(std::nullopt) {}
  constexpr vertex(const point3 &pos, const std::optional<vec3> &uv,
                   const std::optional<vec3> &normal)
      : position(pos), uv(uv), normal(normal) {}
};

struct triangle : public hittable {
  const vertex m_p0, m_p1, m_p2;
  material *m_mat_ptr;
  aabb m_bounding_box;

  constexpr triangle(const vertex &p0, const vertex &p1, const vertex &p2,
                     material *mat)
      : m_p0(p0), m_p1(p1), m_p2(p2), m_mat_ptr(mat) {
    m_bounding_box.merge(p0.position);
    m_bounding_box.merge(p1.position);
    m_bounding_box.merge(p2.position);
    m_bounding_box.expand(eps);
  }

  virtual ~triangle() {}

  virtual bool hit(const ray &r, const real t_min, const real t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(const real time0, const real time1,
                            aabb &output_box) const override;
};
