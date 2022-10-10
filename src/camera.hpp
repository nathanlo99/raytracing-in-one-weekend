
#pragma once

#include "ray.hpp"
#include "util.hpp"

#include <cmath>

struct camera {
  int m_image_width, m_image_height;
  real m_time0, m_time1;
  point3 m_origin;
  point3 m_upper_left_corner;
  vec3 m_horizontal;
  vec3 m_vertical;
  vec3 m_u, m_v, m_w;
  real m_lens_radius;

  camera(const int image_width, const int image_height, const point3 &look_from,
         const point3 &look_at, const vec3 &up, const real vfov,
         const real aspect_ratio, const real aperture, const real focus_dist,
         const real t0, const real t1)
      : m_image_width(image_width), m_image_height(image_height), m_time0(t0),
        m_time1(t1) {
    const real theta = util::degrees_to_radians(vfov);
    const real h = std::tan(theta / 2.0);
    const real viewport_height = 2.0 * h;
    const real viewport_width = aspect_ratio * viewport_height;

    m_w = glm::normalize(look_from - look_at);
    m_u = glm::normalize(glm::cross(up, m_w));
    m_v = glm::cross(m_w, m_u);

    const real two = 2.0;

    m_origin = look_from;
    m_horizontal = focus_dist * viewport_width * m_u;
    m_vertical = focus_dist * viewport_height * m_v;
    m_upper_left_corner =
        m_origin - m_horizontal / two + m_vertical / two - focus_dist * m_w;

    m_lens_radius = aperture / two;
  }

  ray get_ray(const real s, const real t) const {
    const vec3 rd = m_lens_radius * util::random_in_unit_disk();
    const vec3 offset = m_u * rd.x + m_v * rd.y;

    return ray(m_origin + offset,
               m_upper_left_corner + s * m_horizontal - t * m_vertical -
                   m_origin - offset,
               util::random_real(m_time0, m_time1));
  }

  // Returns a deterministic ray, for testing
  ray get_debug_ray(const real s, const real t) const {
    const vec3 direction =
        m_upper_left_corner + s * m_horizontal - t * m_vertical - m_origin;
    return ray(m_origin, direction, 0.0);
  }
};
