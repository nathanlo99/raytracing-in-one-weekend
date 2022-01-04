

#include "image.hpp"
#include "colour.hpp"
#include "util.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <fstream>
#include <string>
#include <vector>

image::image(const std::string &filename) {
  int components_per_pixel = bytes_per_pixel;
  data = stbi_load(("../res/" + filename).c_str(), &width, &height,
                   &components_per_pixel, components_per_pixel);
  if (data == nullptr) {
    std::cerr << "ERROR: Could not load texture image file '" << filename << "'"
              << std::endl;
    width = height = 0;
  }
}

void image::write_ppm(const std::string &filename) {
  std::ofstream out(filename);
  out << "P3\n" << width << ' ' << height << "\n255\n";
  for (int i = 0; i < width * height; ++i) {
    out << static_cast<int>(data[3 * i + 0]) << ' '
        << static_cast<int>(data[3 * i + 1]) << ' '
        << static_cast<int>(data[3 * i + 2]) << '\n';
  }
}

void image::write_png(const std::string &filename) {
  const int result =
      stbi_write_png(filename.c_str(), width, height, bytes_per_pixel, data,
                     bytes_per_pixel * width);
  assert(result != 0);
}
