
#pragma once

#include "util.hpp"

#include <iostream>

inline constexpr real gamma_correct_real(const real d) {
  constexpr real gamma = 2.2, gamma_exp = 1.0 / gamma;
  return pow(d, gamma_exp);
}

inline constexpr colour gamma_correct(const colour &c) {
  return colour(gamma_correct_real(c[0]), gamma_correct_real(c[1]),
                gamma_correct_real(c[2]));
}

inline constexpr unsigned char to_byte(const real d) { return 255.0 * d; }

inline constexpr real from_byte(const unsigned char c) { return c / 255.0; }
