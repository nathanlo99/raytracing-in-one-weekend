
#pragma once

#include "ray.hpp"
#include "util.hpp"

#include <cmath>

struct camera {
  int image_width, image_height;
  real t0, t1;
  point3 origin;
  point3 upper_left_corner;
  vec3 horizontal;
  vec3 vertical;
  vec3 u, v, w;
  real lens_radius;

  camera(const int image_width, const int image_height, const point3 &look_from,
         const point3 &look_at, const vec3 &up, const real vfov,
         const real aspect_ratio, const real aperture, const real focus_dist,
         const real t0, const real t1)
      : image_width(image_width), image_height(image_height), t0(t0), t1(t1) {
    const real theta = util::degrees_to_radians(vfov);
    const real h = std::tan(theta / 2.0);
    const real viewport_height = 2.0 * h;
    const real viewport_width = aspect_ratio * viewport_height;

    w = glm::normalize(look_from - look_at);
    u = glm::normalize(glm::cross(up, w));
    v = glm::cross(w, u);

    const real two = 2.0;

    origin = look_from;
    horizontal = focus_dist * viewport_width * u;
    vertical = focus_dist * viewport_height * v;
    upper_left_corner =
        origin - horizontal / two + vertical / two - focus_dist * w;

    lens_radius = aperture / two;
  }

  ray get_ray(const real s, const real t) const {
    const vec3 rd = lens_radius * util::random_in_unit_disk();
    const vec3 offset = u * rd.x + v * rd.y;

    return ray(origin + offset,
               upper_left_corner + s * horizontal - t * vertical - origin -
                   offset,
               util::random_real(t0, t1));
  }
};
