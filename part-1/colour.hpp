
#pragma once

#include "vec3.hpp"

#include <iostream>

void write_colour(std::ostream &out, const colour &pixel_colour) {
  // Write the translated [0,255] value of each colour component.
  out << static_cast<int>(255.999 * pixel_colour.x()) << ' '
      << static_cast<int>(255.999 * pixel_colour.y()) << ' '
      << static_cast<int>(255.999 * pixel_colour.z()) << '\n';
}
