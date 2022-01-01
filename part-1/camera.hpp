
#pragma once

#include "util.hpp"

class camera {
private:
  point3 origin;
  point3 upper_left_corner;
  vec3 horizontal;
  vec3 vertical;

public:
  constexpr camera() {
    const double aspect_ratio = 16.0 / 9.0;
    const double viewport_height = 2.0;
    const double viewport_width = aspect_ratio * viewport_height;
    const double focal_length = 1.0;

    origin = point3(0, 0, 0);
    horizontal = vec3(viewport_width, 0, 0);
    vertical = vec3(0, viewport_height, 0);
    upper_left_corner =
        origin - horizontal / 2.0 + vertical / 2.0 - vec3(0, 0, focal_length);
  }

  constexpr ray get_ray(double u, double v) const {
    return ray(origin,
               upper_left_corner + u * horizontal - v * vertical - origin);
  }
};
