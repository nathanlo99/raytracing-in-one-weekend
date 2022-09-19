
#include "util.hpp"
#include <glm/glm.hpp>

using vec3 = glm::vec3;
using point3 = glm::vec3;
using colour = glm::vec3;

namespace util {
inline size_t largest_axis(const vec3 &v) {
  if (v[0] >= v[1] && v[0] >= v[2])
    return 0;
  if (v[1] >= v[2])
    return 1;
  return 2;
}

inline vec3 random_vec3() {
  return vec3(random_float(), random_float(), random_float());
}

inline vec3 random_vec3(const float min, const float max) {
  return vec3(random_float(min, max), random_float(min, max),
              random_float(min, max));
}

inline vec3 random_in_unit_disk() {
  while (true) {
    const vec3 p = vec3(random_float(-1, 1), random_float(-1, 1), 0);
    if (dot(p, p) <= 1)
      return p;
  }
}

inline vec3 random_in_unit_sphere() {
  while (true) {
    const vec3 p = random_vec3(-1, 1);
    if (glm::dot(p, p) <= 1)
      return p;
  }
}

inline vec3 random_unit_vector() { return normalize(random_in_unit_sphere()); }

inline bool near_zero(const vec3 &v) { return glm::dot(v, v) < eps * eps; }

}; // namespace util
