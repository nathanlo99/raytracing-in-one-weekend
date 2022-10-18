
#pragma once

#include "ray.hpp"
#include "util.hpp"

#include <array>
struct aabb {
  point3 min, max;

  constexpr aabb(const point3 &min, const point3 &max) : min(min), max(max) {}
  constexpr aabb() : min(inf), max(-inf) {}

  constexpr inline bool does_hit(const ray &r, const real t_min,
                                 const real t_max) const {
    const vec3 ray_hit_min = (min - r.orig) / r.dir,
               ray_hit_max = (max - r.orig) / r.dir;
    const vec3 left_endpoints = glm::min(ray_hit_max, ray_hit_min);
    const vec3 right_endpoints = ray_hit_min + ray_hit_max - left_endpoints;
    const real left = std::max(std::max(left_endpoints.x, left_endpoints.y),
                               std::max(left_endpoints.z, t_min));
    const real right = std::min(std::min(right_endpoints.x, right_endpoints.y),
                                std::min(right_endpoints.z, t_max));
    return right > left;
  }

  constexpr size_t largest_axis() const {
    // TODO: Just inline this
    return util::largest_axis(max - min);
  }

  constexpr inline real hit(const ray &r, const real t_min,
                            const real t_max) const {
    const vec3 ray_hit_min = (min - r.orig) / r.dir,
               ray_hit_max = (max - r.orig) / r.dir;
    const vec3 left_endpoints = glm::min(ray_hit_min, ray_hit_max);
    const vec3 right_endpoints = ray_hit_min + ray_hit_max - left_endpoints;
    const real left = std::max(std::max(left_endpoints.x, left_endpoints.y),
                               std::max(left_endpoints.z, t_min));
    const real right = std::min(std::min(right_endpoints.x, right_endpoints.y),
                                std::min(right_endpoints.z, t_max));
    return right > left ? left : inf;
  }

  constexpr std::array<vec3, 8> corners() const {
    const real x0 = min.x, y0 = min.y, z0 = min.z;
    const real x1 = max.x, y1 = max.y, z1 = max.z;
    return {vec3(x0, y0, z0), vec3(x0, y0, z1), vec3(x0, y1, z0),
            vec3(x0, y1, z1), vec3(x1, y0, z0), vec3(x1, y0, z1),
            vec3(x1, y1, z0), vec3(x1, y1, z1)};
  }

  constexpr vec3 centroid() const { return (min + max) / 2.0; }

  constexpr aabb apply(const mat4 &trans) const {
    vec3 new_min(inf), new_max(-inf);
    for (const vec3 &corner : corners()) {
      const vec3 transformed_corner = trans * vec4(corner, 1.0);
      new_min = glm::min(new_min, transformed_corner);
      new_max = glm::max(new_max, transformed_corner);
    }
    return aabb(new_min, new_max);
  }

  constexpr void merge(const aabb &other) {
    min = glm::min(min, other.min);
    max = glm::max(max, other.max);
  }

  constexpr void merge(const point3 &point) {
    min = glm::min(min, point);
    max = glm::max(max, point);
  }

  constexpr real surface_area() const {
    const vec3 size = max - min;
    const real x = size.x, y = size.y, z = size.z;
    return 2.0 * (x * y + x * z + y * z);
  }

  constexpr void expand(const real amt) {
    const vec3 diff = vec3(amt);
    min -= diff;
    max += diff;
  }
};

constexpr inline aabb surrounding_box(const aabb &box0, const aabb &box1) {
  const point3 small = glm::min(box0.min, box1.min);
  const point3 big = glm::max(box0.max, box1.max);
  return aabb(small, big);
}
