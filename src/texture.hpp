
#pragma once

#include "image.hpp"
#include "util.hpp"

struct texture {
  virtual colour value(const float u, const float v,
                       const point3 &p) const = 0;
};

struct solid_colour : public texture {
  colour colour_value;

  solid_colour(const colour &c) : colour_value(c) {}
  solid_colour(const float r, const float g, const float b)
      : colour_value(r, g, b) {}
  virtual ~solid_colour() = default;

  virtual inline colour value(const float u, const float v,
                              const vec3 &p) const override {
    return colour_value;
  }
};

struct image_texture : public texture {
  image img;

  image_texture(const std::string &filename) : img(filename) {}
  virtual ~image_texture() = default;

  virtual inline colour value(const float u, const float v,
                              const vec3 &p) const override {
    return img.get_interpolated(u, v);
  }
};
