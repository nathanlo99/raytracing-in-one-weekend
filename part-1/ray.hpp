
#pragma once

#include "vec3.hpp"

struct ray {
  point3 orig;
  vec3 dir;

  constexpr ray() = default;
  constexpr ray(const point3 &origin, const vec3 &direction)
      : orig(origin), dir(direction) {}

  constexpr point3 origin() const { return orig; }
  constexpr vec3 direction() const { return dir; }

  constexpr point3 at(const double t) const { return orig + t * dir; }
};
