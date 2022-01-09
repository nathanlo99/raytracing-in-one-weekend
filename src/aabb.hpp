
#pragma once

#include "util.hpp"

struct aabb {
  point3 min, max;

  constexpr aabb(const point3 &min, const point3 &max) : min(min), max(max) {}
  constexpr aabb() : min(infinity), max(-infinity) {}

  constexpr inline bool hit(const ray &r, double t_min, double t_max) const {
    for (int a = 0; a < 3; a++) {
      const double invD = 1.0f / r.dir[a];
      double t0 = (min[a] - r.orig[a]) * invD;
      double t1 = (max[a] - r.orig[a]) * invD;
      if (invD < 0.0f)
        std::swap(t0, t1);
      t_min = t0 > t_min ? t0 : t_min;
      t_max = t1 < t_max ? t1 : t_max;
      if (t_max <= t_min)
        return false;
    }
    return true;
  }
};

constexpr inline aabb surrounding_box(const aabb &box0, const aabb &box1) {
  const point3 small = min(box0.min, box1.min);
  const point3 big = max(box0.max, box1.max);
  return aabb(small, big);
}
