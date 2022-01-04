
#pragma once

#include <chrono>
#include <cmath>
#include <limits>
#include <memory>
#include <random>

// Usings

using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants

constexpr double infinity = std::numeric_limits<double>::infinity();
constexpr double pi = 3.1415926535897932385;
constexpr double eps = 0.0001;

// Utility Functions

constexpr inline double degrees_to_radians(double degrees) {
  return degrees * pi / 180.0;
}

inline float frand(int *seed) {
  union {
    float fres;
    unsigned int ires;
  };

  seed[0] *= 16807;
  ires = (((unsigned int)seed[0]) >> 9) | 0x3f800000;
  return fres - 1.0f;
}

inline double random_double() {
  static int seed = 127;
  return frand(&seed);
}

inline double random_double(const double min, const double max) {
  // Returns a random real in [min, max).
  return min + (max - min) * random_double();
}

inline int random_int(const int min, const int max) {
  // Returns a random integer in [min, max].
  return static_cast<int>(random_double(min, max + 1));
}

inline long long get_time_ms() {
  return std::chrono::duration_cast<std::chrono::milliseconds>(
             std::chrono::system_clock::now().time_since_epoch())
      .count();
}

// Common Headers

#include "ray.hpp"
#include "vec3.hpp"
