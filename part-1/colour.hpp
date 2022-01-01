
#pragma once

#include "util.hpp"
#include "vec3.hpp"

#include <iostream>

void write_colour(std::ostream &out, colour pixel_colour,
                  int samples_per_pixel) {
  // Write the translated [0,255] value of each colour component.
  pixel_colour /= static_cast<double>(samples_per_pixel);
  out << static_cast<int>(256 * clamp(pixel_colour.x(), 0.0, 0.999)) << ' '
      << static_cast<int>(256 * clamp(pixel_colour.y(), 0.0, 0.999)) << ' '
      << static_cast<int>(256 * clamp(pixel_colour.z(), 0.0, 0.999)) << '\n';
}
