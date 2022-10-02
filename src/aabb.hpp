
#pragma once

#include "ray.hpp"
#include "util.hpp"

struct aabb {
  point3 min, max;

  constexpr aabb(const point3 &min, const point3 &max) : min(min), max(max) {}
  constexpr aabb() : min(inf), max(-inf) {}

  constexpr inline bool does_hit(const ray &r, real t_min, real t_max) const {
    for (int a = 0; a < 3; a++) {
      real t0 = (min[a] - r.orig[a]) / r.dir[a];
      real t1 = (max[a] - r.orig[a]) / r.dir[a];
      if (r.dir[a] < 0.0f)
        std::swap(t0, t1);
      t_min = std::max(t0, t_min);
      t_max = std::min(t1, t_max);
      if (t_max <= t_min)
        return false;
    }
    return t_max > t_min;
  }

  constexpr inline real hit(const ray &r, real t_min, real t_max) const {
    for (int a = 0; a < 3; a++) {
      real t0 = (min[a] - r.orig[a]) / r.dir[a];
      real t1 = (max[a] - r.orig[a]) / r.dir[a];
      if (r.dir[a] < 0.0f)
        std::swap(t0, t1);
      t_min = std::max(t0, t_min);
      t_max = std::min(t1, t_max);
    }
    return (t_max > t_min) ? t_min : inf;
  }
};

constexpr inline aabb surrounding_box(const aabb &box0, const aabb &box1) {
  const point3 small = min(box0.min, box1.min);
  const point3 big = max(box0.max, box1.max);
  return aabb(small, big);
}
