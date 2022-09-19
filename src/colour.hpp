
#pragma once

#include "util.hpp"


#include <iostream>

inline constexpr float gamma_correct_float(const float d) {
  constexpr float gamma = 2.2, gamma_exp = 1.0 / gamma;
  return pow(d, gamma_exp);
}

inline constexpr colour gamma_correct(const colour &c) {
  return colour(gamma_correct_float(c[0]), gamma_correct_float(c[1]),
                gamma_correct_float(c[2]));
}

inline unsigned char to_byte(const float d) { return 255.0 * d; }

inline float from_byte(const unsigned char c) { return c / 255.0; }
