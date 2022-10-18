
#include "image.hpp"
#include "colour.hpp"
#include "stb.hpp"
#include "util.hpp"

#include <fstream>
#include <string>
#include <vector>

image::image(const std::string_view &filename) {
  int components_per_pixel = bytes_per_pixel;
  float *loaded_data = stbi_loadf(filename.data(), &m_width, &m_height,
                                  &components_per_pixel, components_per_pixel);
  if (loaded_data == nullptr) {
    std::cerr << "ERROR: Could not load texture image file '" << filename << "'"
              << std::endl;
    std::cerr << stbi_failure_reason() << std::endl;
    return;
  }

  std::cout << "Loaded image '" << filename << "' with size " << m_width
            << " by " << m_height << std::endl;
  m_pixels.resize(m_width * m_height);
  for (int idx = 0; idx < m_width * m_height; ++idx) {
    m_pixels[idx] = colour(loaded_data[3 * idx + 0], loaded_data[3 * idx + 1],
                           loaded_data[3 * idx + 2]);
  }
  stbi_image_free(loaded_data);
}

std::vector<unsigned char> image::gamma_corrected_bytes() const {
  std::vector<unsigned char> gamma_corrected_data(4 * m_width * m_height);
  for (int i = 0; i < m_width * m_height; ++i) {
    gamma_corrected_data[4 * i + 0] = to_byte(
        gamma_correct_real(std::clamp<float>(m_pixels[i][0], 0.0, 1.0)));
    gamma_corrected_data[4 * i + 1] = to_byte(
        gamma_correct_real(std::clamp<float>(m_pixels[i][1], 0.0, 1.0)));
    gamma_corrected_data[4 * i + 2] = to_byte(
        gamma_correct_real(std::clamp<float>(m_pixels[i][2], 0.0, 1.0)));
    gamma_corrected_data[4 * i + 3] = 255;
  }
  return gamma_corrected_data;
}

void image::write_png(const std::string_view &filename) const {
  const std::vector<unsigned char> pixels = gamma_corrected_bytes();
  const int result = stbi_write_png(filename.data(), m_width, m_height, 4,
                                    pixels.data(), 4 * m_width);
  if (result == 0) {
    std::cerr << "write_png(" << filename << ") failed" << std::endl;
    assert(false);
  }
}

void image::draw_to_screen_texture() const {
  std::cout << "Drawing to screen texture" << std::endl;
  const std::vector<unsigned char> pixels = gamma_corrected_bytes();
  std::cout << "Done extracting bytes" << std::endl;
  std::cout << "pixels has size " << pixels.size() << std::endl;
  std::cout << "Uploading texture with width = " << m_width
            << " and height = " << m_height << std::endl;
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
  glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
  glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, &pixels[0]);
  std::cout << "Done drawing to screen texture" << std::endl;
}