
#pragma once

#include "colour.hpp"
#include "util.hpp"

#include <string>
#include <vector>

struct image {
  int width;
  int height;
  std::vector<unsigned char> data;
  const static int bytes_per_pixel = 3;

  image(const std::string &filename);
  image(const int width, const int height, const unsigned char fill_byte = 0)
      : width(width), height(height),
        data(bytes_per_pixel * width * height, fill_byte) {}

  constexpr inline colour get(const int row, const int col) const {
    // Take row mod height and col mod width to wrap the texture on overflow
    const int idx = (row % height) * width + (col % width);
    return colour(from_byte(data[bytes_per_pixel * idx + 0]),
                  from_byte(data[bytes_per_pixel * idx + 1]),
                  from_byte(data[bytes_per_pixel * idx + 2]));
  }

  constexpr inline colour get_floored(double u, double v) const {
    if (data.empty())
      return colour(0, 1, 1);

    u = std::clamp(u, 0.0, 1.0) * width;
    v = (1.0 - std::clamp(v, 0.0, 1.0)) * height;

    return get(static_cast<int>(v), static_cast<int>(u));
  }

  // Bilinear interpolation
  constexpr inline colour get_interpolated(double u, double v) const {
    if (data.empty())
      return colour(0, 1, 1);

    u = std::clamp(u, 0.0, 1.0) * width;
    v = (1.0 - std::clamp(v, 0.0, 1.0)) * height;

    double floor_ud = 0.0, floor_vd = 0.0;
    const double frac_u = modf(u, &floor_ud), frac_v = modf(v, &floor_vd);
    const int floor_u = static_cast<int>(floor_ud);
    const int floor_v = static_cast<int>(floor_vd);

    return frac_u * frac_v * get(floor_v, floor_u) +
           (1 - frac_u) * frac_v * get(floor_v, floor_u + 1) +
           frac_u * (1 - frac_v) * get(floor_v + 1, floor_u) +
           (1 - frac_u) * (1 - frac_v) * get(floor_v + 1, floor_u + 1);
  }

  constexpr inline void set(const int row, const int col, const colour &c) {
    const int idx = row * width + col;
    const colour gamma_corrected = gamma_correct(c);
    data[bytes_per_pixel * idx + 0] = to_byte(gamma_corrected[0]);
    data[bytes_per_pixel * idx + 1] = to_byte(gamma_corrected[1]);
    data[bytes_per_pixel * idx + 2] = to_byte(gamma_corrected[2]);
  }

  void write_ppm(const std::string &filename);
  void write_png(const std::string &filename);
};
