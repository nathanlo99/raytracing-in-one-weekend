
#pragma once

#include "util.hpp"
#include "vec3.hpp"

#include <iostream>

inline colour gamma_correct(const colour &c) {
  const double gamma = 2.2, gamma_exp = 1.0 / gamma;
  return colour(pow(c[0], gamma_exp), pow(c[1], gamma_exp),
                pow(c[2], gamma_exp));
}

inline unsigned char to_byte(const double d) { return 255.0 * d; }

inline double from_byte(const unsigned char c) {
  return static_cast<double>(c) / 255.0;
}
