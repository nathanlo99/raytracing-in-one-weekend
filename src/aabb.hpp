
#pragma once

#include "util.hpp"

struct aabb {
  point3 min, max;

  constexpr aabb(const point3 &min, const point3 &max) : min(min), max(max) {}
  constexpr aabb() : min(inf), max(-inf) {}

  constexpr inline bool does_hit(const ray &r, float t_min, float t_max) const {
    for (int a = 0; a < 3; a++) {
      const float invD = 1.0f / r.dir[a];
      float t0 = (min[a] - r.orig[a]) * invD;
      float t1 = (max[a] - r.orig[a]) * invD;
      if (invD < 0.0f)
        std::swap(t0, t1);
      t_min = std::max(t0, t_min);
      t_max = std::min(t1, t_max);
      if (t_max <= t_min)
        return false;
    }
    return true;
  }

  constexpr inline float hit(const ray &r, float t_min, float t_max) const {
    for (int a = 0; a < 3; a++) {
      const float invD = 1.0f / r.dir[a];
      float t0 = (min[a] - r.orig[a]) * invD;
      float t1 = (max[a] - r.orig[a]) * invD;
      if (invD < 0.0f)
        std::swap(t0, t1);
      t_min = std::max(t0, t_min);
      t_max = std::min(t1, t_max);
    }
    if (t_max <= t_min)
      return inf;
    return t_min;
  }
};

constexpr inline aabb surrounding_box(const aabb &box0, const aabb &box1) {
  const point3 small = min(box0.min, box1.min);
  const point3 big = max(box0.max, box1.max);
  return aabb(small, big);
}
