
#include "image.hpp"
#include "colour.hpp"
#include "stb.hpp"
#include "util.hpp"

#include <fstream>
#include <string>
#include <vector>

image::image(const std::string &filename) {
  int components_per_pixel = bytes_per_pixel;
  float *loaded_data = stbi_loadf(filename.c_str(), &width, &height,
                                  &components_per_pixel, components_per_pixel);
  if (loaded_data == nullptr) {
    std::cerr << "ERROR: Could not load texture image file '" << filename << "'"
              << std::endl;
    std::cerr << stbi_failure_reason() << std::endl;
    return;
  }

  std::cout << "Loaded image '" << filename << "' with size " << width << " by "
            << height << std::endl;
  pixels.resize(width * height);
  for (int idx = 0; idx < width * height; ++idx) {
    pixels[idx] = colour(loaded_data[3 * idx + 0], loaded_data[3 * idx + 1],
                         loaded_data[3 * idx + 2]);
  }
  stbi_image_free(loaded_data);
}

void image::write_png(const std::string &filename) {
  std::vector<unsigned char> gamma_corrected_data(bytes_per_pixel * width *
                                                  height);
  for (int i = 0; i < width * height; ++i) {
    gamma_corrected_data[3 * i + 0] =
        to_byte(gamma_correct_real(std::clamp<real>(pixels[i][0], 0.0, 1.0)));
    gamma_corrected_data[3 * i + 1] =
        to_byte(gamma_correct_real(std::clamp<real>(pixels[i][1], 0.0, 1.0)));
    gamma_corrected_data[3 * i + 2] =
        to_byte(gamma_correct_real(std::clamp<real>(pixels[i][2], 0.0, 1.0)));
  }

  const int result =
      stbi_write_png(filename.c_str(), width, height, bytes_per_pixel,
                     gamma_corrected_data.data(), bytes_per_pixel * width);
  assert(result != 0);
}
