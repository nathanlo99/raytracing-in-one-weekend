
#pragma once

#include "util.hpp"

#include <cmath>
#include <iostream>

struct vec3 {
  double e[3];

  constexpr vec3() : e{0, 0, 0} {}
  constexpr vec3(const double x) : e{x, x, x} {}
  constexpr vec3(const double e0, const double e1, const double e2)
      : e{e0, e1, e2} {}

  constexpr double x() const { return e[0]; }
  constexpr double y() const { return e[1]; }
  constexpr double z() const { return e[2]; }

  constexpr vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
  constexpr double operator[](int i) const { return e[i]; }
  constexpr double &operator[](int i) { return e[i]; }

  constexpr vec3 &operator+=(const vec3 &v) {
    e[0] += v.e[0];
    e[1] += v.e[1];
    e[2] += v.e[2];
    return *this;
  }

  constexpr vec3 &operator*=(const double t) {
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    return *this;
  }

  constexpr vec3 &operator/=(const double t) { return *this *= 1.0 / t; }

  constexpr double length() const { return sqrt(length_squared()); }

  constexpr double length_squared() const {
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
  }

  constexpr vec3 normalize() const {
    const double inv_length = 1 / length();
    return vec3(e[0] * inv_length, e[1] * inv_length, e[2] * inv_length);
  }

  constexpr vec3 clamp(const double min, const double max) const {
    return vec3(std::clamp(e[0], min, max), std::clamp(e[1], min, max),
                std::clamp(e[2], min, max));
  }

  inline static vec3 random() {
    return vec3(random_double(), random_double(), random_double());
  }

  inline static vec3 random(const double min, const double max) {
    return vec3(random_double(min, max), random_double(min, max),
                random_double(min, max));
  }

  constexpr inline bool near_zero() const {
    return std::abs(e[0]) < eps && std::abs(e[1]) < eps && std::abs(e[2]) < eps;
  }
};

// Type aliases for vec3
using point3 = vec3; // 3D point
using colour = vec3; // RGB colour

// vec3 Utility Functions
inline std::ostream &operator<<(std::ostream &out, const vec3 &v) {
  return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

constexpr inline vec3 operator+(const vec3 &u, const vec3 &v) {
  return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

constexpr inline vec3 operator-(const vec3 &u, const vec3 &v) {
  return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

constexpr inline vec3 operator*(const vec3 &u, const vec3 &v) {
  return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

constexpr inline vec3 operator*(const double t, const vec3 &v) {
  return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

constexpr inline vec3 operator*(const vec3 &v, const double t) { return t * v; }

constexpr inline vec3 operator/(const vec3 &v, const double t) {
  return (1 / t) * v;
}

constexpr inline vec3 min(const vec3 &u, const vec3 &v) {
  return vec3(std::min(u[0], v[0]), std::min(u[1], v[1]), std::min(u[2], v[2]));
}

constexpr inline vec3 max(const vec3 &u, const vec3 &v) {
  return vec3(std::max(u[0], v[0]), std::max(u[1], v[1]), std::max(u[2], v[2]));
}

constexpr inline double dot(const vec3 &u, const vec3 &v) {
  return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

constexpr inline vec3 cross(const vec3 &u, const vec3 &v) {
  return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
              u.e[2] * v.e[0] - u.e[0] * v.e[2],
              u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

constexpr inline vec3 unit_vector(const vec3 &v) { return v / v.length(); }

inline vec3 random_in_unit_sphere() {
  while (true) {
    const vec3 p = vec3::random(-1, 1);
    if (p.length_squared() < 1)
      return p;
  }
}

inline vec3 random_in_unit_disk() {
  while (true) {
    const vec3 p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
    if (p.length_squared() < 1)
      return p;
  }
}

inline vec3 random_unit_vector() { return random_in_unit_sphere().normalize(); }

constexpr inline vec3 reflect(const vec3 &v, const vec3 &n) {
  return v - 2 * dot(v, n) * n;
}

constexpr inline vec3 refract(const vec3 &uv, const vec3 &n,
                              const double index_ratio) {
  const double cos_theta = std::min(dot(-uv, n), 1.0);
  const vec3 &r_out_perp = index_ratio * (uv + cos_theta * n);
  const vec3 &r_out_parallel =
      -std::sqrt(std::abs(1.0 - r_out_perp.length_squared())) * n;
  return r_out_perp + r_out_parallel;
}
