
#pragma once

#include "util.hpp"

#include <cmath>

class camera {
private:
  point3 origin;
  point3 upper_left_corner;
  vec3 horizontal;
  vec3 vertical;
  vec3 u, v, w;
  double lens_radius;

public:
  camera(const point3 &look_from, const point3 &look_at, const vec3 &up,
         const double vfov, const double aspect_ratio, const double aperture,
         const double focus_dist) {
    const double theta = degrees_to_radians(vfov);
    const double h = std::tan(theta / 2);
    const double viewport_height = 2.0 * h;
    const double viewport_width = aspect_ratio * viewport_height;

    w = (look_from - look_at).normalize();
    u = cross(up, w).normalize();
    v = cross(w, u);

    origin = look_from;
    horizontal = focus_dist * viewport_width * u;
    vertical = focus_dist * viewport_height * v;
    upper_left_corner = origin - horizontal / 2 + vertical / 2 - focus_dist * w;

    lens_radius = aperture / 2;
  }

  ray get_ray(const double s, const double t) const {
    const vec3 rd = lens_radius * random_in_unit_disk();
    const vec3 offset = u * rd.x() + v * rd.y();

    return ray(origin + offset, upper_left_corner + s * horizontal -
                                    t * vertical - origin - offset);
  }
};
