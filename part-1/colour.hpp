
#pragma once

#include "util.hpp"
#include "vec3.hpp"

#include <iostream>

inline colour gamma_correct(const colour &c) {
  const double gamma = 2.2, gamma_exp = 1.0 / gamma;
  return colour(pow(c[0], gamma_exp), pow(c[1], gamma_exp),
                pow(c[2], gamma_exp));
}

void write_colour(std::ostream &out, colour pixel_colour) {
  pixel_colour = gamma_correct(pixel_colour).clamp(0, 1);
  out << static_cast<int>(255.99 * pixel_colour[0]) << ' '
      << static_cast<int>(255.99 * pixel_colour[1]) << ' '
      << static_cast<int>(255.99 * pixel_colour[2]) << '\n';
}
