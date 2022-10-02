
#pragma once

#include <algorithm>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <utility>
#include <vector>

#include <boost/assert.hpp>
#include <boost/random/sobol.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

#include <glm/glm.hpp>

// Usings

using std::make_shared;
using std::shared_ptr;

#if USE_FLOATS
using real = float;
using vec3 = glm::vec3;
#else
using real = double;
using vec3 = glm::dvec3;
#endif

// Constants

constexpr real inf = std::numeric_limits<real>::infinity();
constexpr real pi = 3.1415926535897932385;
constexpr real eps = 0.001;

using point3 = vec3;
using colour = vec3;

// Utility Functions

namespace util {
constexpr inline real degrees_to_radians(real degrees) {
  return degrees * pi / 180.0;
}

inline real random_real() {
  // return rand() / (RAND_MAX + 1.0);
  static std::uniform_real_distribution<real> distribution(0.0, 1.0);
  static std::mt19937 generator(127);
  return distribution(generator);
}

inline real random_real(const real min, const real max) {
  // Returns a random real in [min, max).
  return min + (max - min) * random_real();
}

inline int random_int(const int min, const int max) {
  // Returns a random integer in [min, max].
  return static_cast<int>(random_real(min, max + 1));
}

inline long long get_time_ns() {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

inline long long get_time_ms() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

inline auto get_sobol_sequence(const size_t dimensions,
                               const size_t num_samples) {
  boost::random::sobol generator(dimensions);
  boost::uniform_real<> uni_dist(0.0, 1.0);
  boost::variate_generator<boost::random::sobol &, boost::uniform_real<>> uni(
      generator, uni_dist);

  std::vector<std::pair<real, real>> result(num_samples);
  for (size_t i = 0; i < num_samples; ++i)
    result[i] = std::make_pair(uni(), uni());
  return result;
}

inline size_t largest_axis(const vec3 &v) {
  if (v[0] >= v[1] && v[0] >= v[2])
    return 0;
  if (v[1] >= v[2])
    return 1;
  return 2;
}

inline vec3 random_vec3() {
  return vec3(random_real(), random_real(), random_real());
}

inline vec3 random_vec3(const real min, const real max) {
  return vec3(random_real(min, max), random_real(min, max),
              random_real(min, max));
}

inline vec3 random_in_unit_disk() {
  while (true) {
    const vec3 p = vec3(random_real(-1, 1), random_real(-1, 1), 0);
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
