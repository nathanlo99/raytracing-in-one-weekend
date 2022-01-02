

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
  const int result = stbi_write_png(filename.c_str(), width, height, 3,
                                    data.data(), 3 * width);
  assert(result != 0);
}
