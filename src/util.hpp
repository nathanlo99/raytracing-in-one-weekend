
#pragma once

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <limits>
#include <memory>
#include <random>
#include <utility>
#include <vector>

#include <boost/random/sobol.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/variate_generator.hpp>

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

// inline float frand(int *seed) {
//   union {
//     float fres;
//     unsigned int ires;
//   };
//
//   seed[0] *= 16807;
//   ires = (((unsigned int)seed[0]) >> 9) | 0x3f800000;
//   return fres - 1.0f;
// }
//
// inline double random_double() {
//   static int seed = 127;
//   return frand(&seed);
// }

inline double random_double() {
  static std::uniform_real_distribution<double> distribution(0.0, 1.0);
  static std::mt19937 generator(127);
  return distribution(generator);
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

inline auto get_sobol_sequence(const size_t dimensions,
                               const size_t num_samples) {
  boost::random::sobol generator(dimensions);
  boost::uniform_real<> uni_dist(0.0, 1.0);
  boost::variate_generator<boost::random::sobol &, boost::uniform_real<>> uni(
      generator, uni_dist);

  std::vector<std::pair<double, double>> result(num_samples);
  for (size_t i = 0; i < num_samples; ++i) {
    const double x = uni(), y = uni();
    result[i] = std::make_pair(x, y);
  }
  return result;
}

// Common Headers
#include "ray.hpp"
#include "vec3.hpp"
