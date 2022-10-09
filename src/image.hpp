
#pragma once

#include "colour.hpp"
#include "util.hpp"

#include <string>
#include <vector>

struct image {
  int m_width, m_height;
  std::vector<colour> m_pixels;

  const static int bytes_per_pixel = 3;

  image(const std::string_view &filename);
  image(const int width, const int height)
      : m_width(width), m_height(height), m_pixels(width * height) {}

  inline colour get(const int row, const int col) const {
    // Take row mod height and col mod width to wrap the texture on overflow
    const size_t idx = (row % m_height) * m_width + (col % m_width);
    return m_pixels[idx];
  }

  inline colour get_floored(real u, real v) const {
    if (m_pixels.empty()) {
      // Return magenta to identify loading errors more easily
      return colour(1.0, 0.0, 1.0);
    }

    u = std::clamp<real>(u, 0.0, 1.0) * m_width;
    v = (1.0 - std::clamp<real>(v, 0.0, 1.0)) * m_height;

    return get(static_cast<int>(v), static_cast<int>(u));
  }

  // Bilinear interpolation
  inline colour get_interpolated(const real u, const real v) const {
    if (m_pixels.empty())
      return colour(1.0, 0.0, 1.0);

    const real scaled_u = std::clamp<real>(u, 0.0, 1.0) * m_width;
    const real scaled_v = (1.0 - std::clamp<real>(v, 0.0, 1.0)) * m_height;

    real floor_ud = 0.0, floor_vd = 0.0;
    const real frac_u = modf(scaled_u, &floor_ud),
               frac_v = modf(scaled_v, &floor_vd);
    const int floor_u = static_cast<int>(floor_ud);
    const int floor_v = static_cast<int>(floor_vd);

    return frac_u * frac_v * get(floor_v, floor_u) +
           (1 - frac_u) * frac_v * get(floor_v, floor_u + 1) +
           frac_u * (1 - frac_v) * get(floor_v + 1, floor_u) +
           (1 - frac_u) * (1 - frac_v) * get(floor_v + 1, floor_u + 1);
  }

  inline void set(const int row, const int col, const colour &c) {
    const size_t idx = row * m_width + col;
    m_pixels[idx] = c;
  }

  void write_png(const std::string_view &filename);
};
