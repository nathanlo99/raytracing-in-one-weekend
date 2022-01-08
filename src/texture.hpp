
#pragma once

#include "util.hpp"

struct texture {
  virtual colour value(const double u, const double v,
                       const point3 &p) const = 0;
};

struct solid_colour : public texture {
  colour colour_value;

  solid_colour(const colour &c) : colour_value(c) {}
  solid_colour(const double r, const double g, const double b)
      : colour_value(r, g, b) {}
  virtual ~solid_colour() = default;

  virtual colour value(const double u, const double v,
                       const vec3 &p) const override {
    return colour_value;
  }
};

struct image_texture : public texture {
  image img;
  const double mult;

  image_texture(const std::string &filename, const double mult = 1.0)
      : img(filename), mult(mult) {}
  virtual ~image_texture() = default;

  virtual colour value(const double u, const double v,
                       const vec3 &p) const override {
    return mult * img.get_interpolated(u, v);
  }
};
