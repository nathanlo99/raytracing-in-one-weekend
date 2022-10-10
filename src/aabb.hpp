
#pragma once

#include "ray.hpp"
#include "util.hpp"

#include <array>
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

  constexpr std::array<vec3, 8> corners() const {
    const real x0 = min.x, y0 = min.y, z0 = min.z;
    const real x1 = max.x, y1 = max.y, z1 = max.z;
    return {vec3(x0, y0, z0), vec3(x0, y0, z1), vec3(x0, y1, z0),
            vec3(x0, y1, z1), vec3(x1, y0, z0), vec3(x1, y0, z1),
            vec3(x1, y1, z0), vec3(x1, y1, z1)};
  }

  constexpr aabb apply(const mat4 &trans) const {
    vec3 new_min(inf), new_max(-inf);
    for (const vec3 corner : corners()) {
      const vec3 transformed_corner = trans * vec4(corner, 1.0);
      new_min = glm::min(new_min, transformed_corner);
      new_max = glm::max(new_max, transformed_corner);
    }
    return aabb(new_min, new_max);
  }
};

constexpr inline aabb surrounding_box(const aabb &box0, const aabb &box1) {
  const point3 small = glm::min(box0.min, box1.min);
  const point3 big = glm::max(box0.max, box1.max);
  return aabb(small, big);
}
