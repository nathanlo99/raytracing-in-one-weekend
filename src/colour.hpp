
#pragma once

#include "util.hpp"
#include "vec3.hpp"

#include <iostream>

inline constexpr double gamma_correct_double(const double d) {
  constexpr double gamma = 2.2, gamma_exp = 1.0 / gamma;
  return pow(d, gamma_exp);
}

inline constexpr colour gamma_correct(const colour &c) {
  return colour(gamma_correct_double(c[0]), gamma_correct_double(c[1]),
                gamma_correct_double(c[2]));
}

inline unsigned char to_byte(const double d) { return 255.0 * d; }

inline double from_byte(const unsigned char c) { return c / 255.0; }
