
#pragma once

#include "util.hpp"

#include "texture.hpp"

struct hit_record;

struct material {
  virtual ~material() = default;
  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       colour &attenuation, ray &scattered) const = 0;
  virtual colour emitted(const float u, const float v, const point3 &p) const {
    return colour(0.0f, 0.0f, 0.0f);
  }
};

struct lambertian : public material {
  shared_ptr<texture> albedo;

  lambertian(const colour &a) : albedo(make_shared<solid_colour>(a)) {}
  lambertian(shared_ptr<texture> a) : albedo(a) {}

  virtual ~lambertian() = default;

  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       colour &attenuation, ray &scattered) const override {
    const vec3 scatter_direction = rec.normal + util::random_unit_vector();
    if (util::near_zero(scatter_direction))
      return false; // scatter_direction = rec.normal;

    scattered = ray(rec.p, scatter_direction, r_in.time);
    attenuation = albedo->value(rec.u, rec.v, rec.p);
    return true;
  }
};

struct metal : public material {
  colour albedo;
  float fuzz;

  metal(const colour &a, float f)
      : albedo(a), fuzz(std::clamp<float>(f, 0.0, 1.0)) {}
  virtual ~metal() = default;

  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       colour &attenuation, ray &scattered) const override {
    const vec3 reflected = reflect(glm::normalize(r_in.dir), rec.normal);
    scattered =
        ray(rec.p, reflected + fuzz * util::random_in_unit_sphere(), r_in.time);
    attenuation = albedo;
    return glm::dot(scattered.dir, rec.normal) > 0;
  }
};

struct dielectric : public material {
  colour albedo;
  float index_of_refraction;

  explicit dielectric(const colour &a, const float index_of_refraction)
      : albedo(a), index_of_refraction(index_of_refraction) {}
  virtual ~dielectric() = default;

  static inline float reflectance(const float cosine, const float index_ratio) {
    const float sqrt_r0 = (1 - index_ratio) / (1 + index_ratio);
    const float r0 = sqrt_r0 * sqrt_r0;
    return r0 + (1 - r0) * pow(1 - cosine, 5);
  }

  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       colour &attenuation, ray &scattered) const override {
    attenuation = albedo;
    const float index_ratio =
        rec.front_face ? 1.0 / index_of_refraction : index_of_refraction;

    const vec3 unit_direction = normalize(r_in.dir);
    const float cos_theta =
        std::min<float>(-dot(unit_direction, rec.normal), 1.0);
    const float sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    const bool cannot_reflect = index_ratio * sin_theta > 1.0;
    if (cannot_reflect ||
        random_float() < reflectance(cos_theta, index_ratio)) {
      const vec3 direction = reflect(unit_direction, rec.normal);
      scattered = ray(rec.p, direction, r_in.time);
      return true;
    } else {
      const vec3 direction = refract(unit_direction, rec.normal, index_ratio);
      scattered = ray(rec.p, direction, r_in.time);
      return true;
    }
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

  virtual colour emitted(const float u, const float v,
                         const point3 &p) const override {
    return emit->value(u, v, p);
  }
};
