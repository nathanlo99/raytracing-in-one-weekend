

#include "image.hpp"
#include "colour.hpp"
#include "stb.hpp"
#include "util.hpp"

#include <fstream>
#include <string>
#include <vector>

image::image(const std::string &filename) {
  int components_per_pixel = bytes_per_pixel;
  unsigned char *loaded_data =
      stbi_load(("../res/" + filename).c_str(), &width, &height,
                &components_per_pixel, components_per_pixel);
  if (loaded_data == nullptr) {
    std::cerr << "ERROR: Could not load texture image file '" << filename << "'"
              << std::endl;
  } else {
    data = std::vector<unsigned char>(
        loaded_data, loaded_data + bytes_per_pixel * width * height);
  }
  delete loaded_data;
}

void image::write_png(const std::string &filename) {
  std::vector<unsigned char> gamma_corrected_data(data.size());
  for (size_t i = 0; i < data.size(); ++i) {
    gamma_corrected_data[i] = to_byte(gamma_correct_double(from_byte(data[i])));
  }

  const int result =
      stbi_write_png(filename.c_str(), width, height, bytes_per_pixel,
                     gamma_corrected_data.data(), bytes_per_pixel * width);
  assert(result != 0);
}
