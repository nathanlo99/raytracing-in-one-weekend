
#pragma once

#include "util.hpp"

#include <cmath>

struct camera {
  int image_width, image_height;
  point3 origin;
  point3 upper_left_corner;
  vec3 horizontal;
  vec3 vertical;
  vec3 u, v, w;
  float lens_radius;

  camera(const int image_width, const int image_height, const point3 &look_from,
         const point3 &look_at, const vec3 &up, const float vfov,
         const float aspect_ratio, const float aperture, const float focus_dist)
      : image_width(image_width), image_height(image_height) {
    const float theta = degrees_to_radians(vfov);
    const float h = std::tan(theta / 2);
    const float viewport_height = 2.0 * h;
    const float viewport_width = aspect_ratio * viewport_height;

    w = normalize(look_from - look_at);
    u = normalize(cross(up, w));
    v = cross(w, u);

    const float two = 2.0;

    origin = look_from;
    horizontal = focus_dist * viewport_width * u;
    vertical = focus_dist * viewport_height * v;
    upper_left_corner =
        origin - horizontal / two + vertical / two - focus_dist * w;

    lens_radius = aperture / two;
  }

  ray get_ray(const float s, const float t) const {
    const vec3 rd = lens_radius * util::random_in_unit_disk();
    const vec3 offset = u * rd.x + v * rd.y;

    return ray(origin + offset, upper_left_corner + s * horizontal -
                                    t * vertical - origin - offset);
  }
};
