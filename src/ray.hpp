
#pragma once

#include <vec.hpp>

struct ray {
  point3 orig;
  vec3 dir;
  float time = 0.0;

  constexpr ray() = default;
  constexpr ray(const point3 &origin, const vec3 &direction, const float time)
      : orig(origin), dir(direction), time(time) {}

  constexpr point3 at(const float t) const { return orig + t * dir; }
};
