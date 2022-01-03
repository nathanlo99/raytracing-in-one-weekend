
#pragma once

#include "colour.hpp"
#include "util.hpp"

#include <string>
#include <vector>

struct image {
  int width;
  int height;
  std::vector<unsigned char> data;

  image(const int width, const int height)
      : width(width), height(height), data(3 * width * height) {}

  constexpr inline colour get(const int row, const int col) {
    const int idx = row * width + col;
    return colour(from_byte(data[3 * idx + 0]), from_byte(data[3 * idx + 1]),
                  from_byte(data[3 * idx + 2]));
  }

  constexpr inline void set(const int row, const int col, const colour &c) {
    const int idx = row * width + col;
    const colour gamma_corrected = gamma_correct(c);
    data[3 * idx + 0] = to_byte(gamma_corrected[0]);
    data[3 * idx + 1] = to_byte(gamma_corrected[1]);
    data[3 * idx + 2] = to_byte(gamma_corrected[2]);
  }

  void write_ppm(const std::string &filename);
  void write_png(const std::string &filename);
};
