
#pragma once

#include "util.hpp"

#include <cmath>

struct camera {
  int image_width, image_height;
  double t0, t1;
  point3 origin;
  point3 upper_left_corner;
  vec3 horizontal;
  vec3 vertical;
  vec3 u, v, w;
  double lens_radius;

  camera(const int image_width, const int image_height, const point3 &look_from,
         const point3 &look_at, const vec3 &up, const double vfov,
         const double aspect_ratio, const double aperture,
         const double focus_dist, const double t0, const double t1)
      : image_width(image_width), image_height(image_height), t0(t0), t1(t1) {
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

    return ray(origin + offset,
               upper_left_corner + s * horizontal - t * vertical - origin -
                   offset,
               random_double(t0, t1));
  }
};
