
#pragma once

#include "util.hpp"

#include "hittable.hpp"
#include "sphere.hpp"

struct animated_sphere : public hittable {
  point3 m_centre0 = point3(0.0), m_centre1 = point3(0.0);
  real m_time0 = 0.0, m_time1 = 1.0;
  real m_radius = 1.0;
  material *m_mat_ptr = nullptr;

  constexpr animated_sphere() = default;
  constexpr animated_sphere(const point3 &m_centre0, const point3 &centre1,
                            const real time0, const real time1,
                            const real radius, material *mat_ptr)
      : m_centre0(m_centre0), m_centre1(centre1), m_time0(time0),
        m_time1(time1), m_radius(radius), m_mat_ptr(mat_ptr) {}
  virtual ~animated_sphere() = default;

  virtual bool hit(const ray &r, const real t_min, const real t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(const real time0, const real time1,
                            aabb &output_box) const override;

  constexpr inline point3 get_centre(const real time) const {
    const real t = (time - m_time0) / (m_time1 - m_time0);
    return (1.0 - t) * m_centre0 + t * m_centre1;
  }
};
