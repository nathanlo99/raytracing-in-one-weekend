
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
  std::vector<colour> pixels;

  image(int width, int height)
      : width(width), height(height), pixels(width * height) {}

  inline colour get(int row, int col) {
    row = height - 1 - row;
    return pixels[row * width + col];
  }

  inline void set(int row, int col, const colour &c) {
    row = height - 1 - row;
    pixels[row * width + col] = c;
  }

  void write_ppm(const std::string &filename) {
    std::ofstream out(filename);
    out << "P3\n" << width << ' ' << height << "\n255\n";
    for (const colour &c : pixels) {
      write_colour(out, c);
    }
  }

  void write_png(const std::string &filename) {
    std::vector<unsigned char> data(3 * width * height);
    for (int i = 0; i < pixels.size(); ++i) {
      const colour &corrected = gamma_correct(pixels[i]);
      data[3 * i + 0] = static_cast<unsigned char>(255.99 * corrected[0]);
      data[3 * i + 1] = static_cast<unsigned char>(255.99 * corrected[1]);
      data[3 * i + 2] = static_cast<unsigned char>(255.99 * corrected[2]);
    }
    const int result = stbi_write_png(filename.c_str(), width, height, 3,
                                      data.data(), 3 * width);
    assert(result != 0);
  }
};
