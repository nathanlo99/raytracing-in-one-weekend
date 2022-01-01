
#pragma once

#include "util.hpp"

struct hit_record;

struct material {
  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       colour &attenuation, ray &scattered) const = 0;
};

struct lambertian : public material {
  colour albedo;

  lambertian(const colour &a) : albedo(a) {}

  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       colour &attenuation, ray &scattered) const override {
    vec3 scatter_direction = rec.normal + random_unit_vector();
    if (scatter_direction.near_zero())
      scatter_direction = rec.normal;

    scattered = ray(rec.p, scatter_direction);
    attenuation = albedo;
    return true;
  }
};

struct metal : public material {
  colour albedo;
  double fuzz;

  metal(const colour &a, double f) : albedo(a), fuzz(std::min(1.0, f)) {}

  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       colour &attenuation, ray &scattered) const override {
    vec3 reflected = reflect(r_in.dir.normalized(), rec.normal);
    scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
    attenuation = albedo;
    return dot(scattered.dir, rec.normal) > 0;
  }
};
