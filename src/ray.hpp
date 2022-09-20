
#pragma once

#include <vec.hpp>

struct ray {
  point3 orig;
  vec3 dir;

  constexpr ray() = default;
  constexpr ray(const point3 &origin, const vec3 &direction)
      : orig(origin), dir(direction) {}

  constexpr point3 at(const float t) const { return orig + t * dir; }
};
