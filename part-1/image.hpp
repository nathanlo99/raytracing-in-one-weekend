
#pragma once

#include "colour.hpp"
#include "util.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <fstream>
#include <string>
#include <vector>

struct image {
  int width;
  int height;
  std::vector<unsigned char> data;

  image(const int width, const int height)
      : width(width), height(height), data(3 * width * height) {}

  constexpr inline colour get(int row, int col) {
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

  void write_ppm(const std::string &filename) {
    std::ofstream out(filename);
    out << "P3\n" << width << ' ' << height << "\n255\n";
    for (int i = 0; i < width * height; ++i) {
      out << static_cast<int>(data[3 * i + 0]) << ' '
          << static_cast<int>(data[3 * i + 1]) << ' '
          << static_cast<int>(data[3 * i + 2]) << '\n';
    }
  }

  void write_png(const std::string &filename) {
    const int result = stbi_write_png(filename.c_str(), width, height, 3,
                                      data.data(), 3 * width);
    assert(result != 0);
  }
};
