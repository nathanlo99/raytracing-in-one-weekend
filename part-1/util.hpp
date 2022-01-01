
#pragma once

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

inline double degrees_to_radians(double degrees) {
  return degrees * pi / 180.0;
}

float frand(int *seed) {
  union {
    float fres;
    unsigned int ires;
  };

  seed[0] *= 16807;
  ires = (((unsigned int)seed[0]) >> 9) | 0x3f800000;
  return fres - 1.0f;
}

inline double random_double() {
  static int seed = 42069;
  return frand(&seed);
}

inline double random_double(double min, double max) {
  // Returns a random real in [min,max).
  return min + (max - min) * random_double();
}

inline double clamp(double x, double min, double max) {
  if (x < min)
    return min;
  if (x > max)
    return max;
  return x;
}

// Common Headers

#include "ray.hpp"
#include "vec3.hpp"
