
#pragma once

#include "util.hpp"

struct ray {
  point3 orig;
  vec3 dir;
  real time = 0.0;

  constexpr ray() = default;
  constexpr ray(const point3 &origin, const vec3 &direction, const real time)
      : orig(origin), dir(direction), time(time) {}

  constexpr point3 at(const real t) const { return orig + t * dir; }
};
