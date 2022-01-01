
#pragma once

#include "util.hpp"
#include "vec3.hpp"

#include <iostream>

inline colour gamma_correct(const colour &c) {
  return colour(sqrt(c[0]), sqrt(c[1]), sqrt(c[2]));
}

void write_colour(std::ostream &out, colour pixel_colour,
                  int samples_per_pixel) {
  double r = pixel_colour.x();
  double g = pixel_colour.y();
  double b = pixel_colour.z();

  // Divide the color by the number of samples and gamma-correct for gamma=2.0.
  const double scale = 1.0 / samples_per_pixel;
  const double gamma = 2.0, gamma_exp = 1.0 / gamma;
  r = std::pow(scale * r, gamma_exp);
  g = std::pow(scale * g, gamma_exp);
  b = std::pow(scale * b, gamma_exp);
  out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
      << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
      << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}
