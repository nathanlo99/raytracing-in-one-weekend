
#pragma once

#include "image.hpp"
#include "util.hpp"

struct texture {
  virtual colour value(const real u, const real v, const point3 &p) const = 0;
};

struct solid_colour : public texture {
  const colour m_colour;

  explicit solid_colour(const colour &c) : m_colour(c) {}
  explicit solid_colour(const real r, const real g, const real b)
      : m_colour(r, g, b) {}
  virtual ~solid_colour() = default;

  virtual inline colour value(const real u, const real v,
                              const vec3 &p) const override {
    return m_colour;
  }
};

struct image_texture : public texture {
  const image m_image;

  explicit image_texture(const std::string_view &filename)
      : m_image(filename) {}
  virtual ~image_texture() = default;

  virtual inline colour value(const real u, const real v,
                              const vec3 &p) const override {
    return m_image.get_interpolated(u, v);
  }
};
