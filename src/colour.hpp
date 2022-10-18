
#pragma once

#include "util.hpp"

constexpr inline real gamma_correct_real(const real d) {
  constexpr real gamma = 2.2, gamma_exp = 1.0 / gamma;
  return pow(d, gamma_exp);
}

constexpr inline colour gamma_correct(const colour &c) {
  return colour(gamma_correct_real(c[0]), gamma_correct_real(c[1]),
                gamma_correct_real(c[2]));
}

constexpr inline unsigned char to_byte(const real d) { return 255.0 * d; }

constexpr inline real from_byte(const unsigned char c) { return c / 255.0; }
