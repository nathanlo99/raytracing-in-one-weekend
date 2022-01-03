
#pragma once

#include "vec3.hpp"

struct ray {
  point3 orig;
  vec3 dir;
  double time = 0.0;

  constexpr ray() = default;
  constexpr ray(const point3 &origin, const vec3 &direction, const double time)
      : orig(origin), dir(direction), time(time) {}

  constexpr point3 at(const double t) const { return orig + t * dir; }
};
