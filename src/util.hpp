
#pragma once

#include <algorithm>
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

// Usings

using std::make_shared;
using std::shared_ptr;
using std::sqrt;

// Constants

constexpr float infinity = std::numeric_limits<float>::infinity();
constexpr float pi = 3.1415926535897932385;
constexpr float eps = 0.001;

// Utility Functions

constexpr inline float degrees_to_radians(float degrees) {
  return degrees * pi / 180.0;
}

inline float random_float() {
  static std::uniform_real_distribution<float> distribution(0.0, 1.0);
  static std::mt19937 generator(127);
  return distribution(generator);
}

inline float random_float(const float min, const float max) {
  // Returns a random real in [min, max).
  return min + (max - min) * random_float();
}

inline int random_int(const int min, const int max) {
  // Returns a random integer in [min, max].
  return static_cast<int>(random_float(min, max + 1));
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

  std::vector<std::pair<float, float>> result(num_samples);
  for (size_t i = 0; i < num_samples; ++i)
    result[i] = std::make_pair(uni(), uni());
  return result;
}

// Common Headers
#include "ray.hpp"
