
#pragma once

#include "util.hpp"

#include "texture.hpp"

struct hit_record;

struct material {
  virtual ~material() = default;
  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       colour &attenuation, ray &scattered) const = 0;
  virtual colour emitted(const double u, const double v,
                         const point3 &p) const {
    return colour(0, 0, 0);
  }
};

struct lambertian : public material {
  shared_ptr<texture> albedo;

  lambertian(const colour &a) : albedo(make_shared<solid_colour>(a)) {}
  lambertian(shared_ptr<texture> a) : albedo(a) {}

  virtual ~lambertian() = default;

  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       colour &attenuation, ray &scattered) const override {
    vec3 scatter_direction = rec.normal + random_unit_vector();
    if (scatter_direction.near_zero())
      scatter_direction = rec.normal;

    scattered = ray(rec.p, scatter_direction, r_in.time);
    attenuation = albedo->value(rec.u, rec.v, rec.p);
    return true;
  }
};

struct metal : public material {
  colour albedo;
  double fuzz;

  metal(const colour &a, double f) : albedo(a), fuzz(std::min(1.0, f)) {}
  virtual ~metal() = default;

  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       colour &attenuation, ray &scattered) const override {
    const vec3 reflected = reflect(r_in.dir.normalize(), rec.normal);
    scattered =
        ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time);
    attenuation = albedo;
    return dot(scattered.dir, rec.normal) > 0;
  }
};

struct dielectric : public material {
  colour albedo;
  double index_of_refraction;

  explicit dielectric(const colour &a, double index_of_refraction)
      : albedo(a), index_of_refraction(index_of_refraction) {}
  virtual ~dielectric() = default;

  static inline double reflectance(const double cosine,
                                   const double index_ratio) {
    const double sqrt_r0 = (1 - index_ratio) / (1 + index_ratio);
    const double r0 = sqrt_r0 * sqrt_r0;
    return r0 + (1 - r0) * pow(1 - cosine, 5);
  }

  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       colour &attenuation, ray &scattered) const override {
    attenuation = albedo;
    const double index_ratio =
        rec.front_face ? 1.0 / index_of_refraction : index_of_refraction;

    const vec3 unit_direction = r_in.dir.normalize();
    const double cos_theta = std::min(dot(-unit_direction, rec.normal), 1.0);
    const double sin_theta = std::sqrt(1 - cos_theta * cos_theta);

    const bool cannot_reflect = index_ratio * sin_theta > 1.0;
    vec3 direction;
    if (cannot_reflect ||
        random_double() < reflectance(cos_theta, index_ratio)) {
      direction = reflect(unit_direction, rec.normal);
    } else {
      direction = refract(unit_direction, rec.normal, index_ratio);
    }

    scattered = ray(rec.p, direction, r_in.time);
    return true;
  }
};

struct diffuse_light : public material {
  shared_ptr<texture> emit;

  diffuse_light(shared_ptr<texture> a) : emit(a) {}
  diffuse_light(const colour &a) : emit(make_shared<solid_colour>(a)) {}

  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       colour &attenuation, ray &scattered) const override {
    return false;
  }

  virtual colour emitted(const double u, const double v,
                         const point3 &p) const override {
    return emit->value(u, v, p);
  }
};
