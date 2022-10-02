
#pragma once

#include "image.hpp"
#include "util.hpp"

struct texture {
  virtual colour value(const real u, const real v, const point3 &p) const = 0;
};

struct solid_colour : public texture {
  colour colour_value;

  explicit solid_colour(const colour &c) : colour_value(c) {}
  explicit solid_colour(const real r, const real g, const real b)
      : colour_value(r, g, b) {}
  virtual ~solid_colour() = default;

  virtual inline colour value(const real u, const real v,
                              const vec3 &p) const override {
    return colour_value;
  }
};

struct image_texture : public texture {
  image img;

  explicit image_texture(const std::string &filename) : img(filename) {}
  virtual ~image_texture() = default;

  virtual inline colour value(const real u, const real v,
                              const vec3 &p) const override {
    return img.get_interpolated(u, v);
  }
};
