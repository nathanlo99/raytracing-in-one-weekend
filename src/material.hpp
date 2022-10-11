
#pragma once

#include "hittable.hpp"
#include "util.hpp"

#include "ray.hpp"
#include "texture.hpp"

struct hit_record;

struct material {
  virtual ~material() = default;
  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       colour &attenuation, ray &scattered) const = 0;
  virtual colour emitted(const real u, const real v, const point3 &p) const {
    return colour(0.0f, 0.0f, 0.0f);
  }
};

struct lambertian : public material {
  std::shared_ptr<texture> albedo;

  explicit lambertian(const colour &a)
      : albedo(std::make_shared<solid_colour>(a)) {}
  explicit lambertian(const std::shared_ptr<texture> &a) : albedo(a) {}

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
  colour albedo = colour(1.0);
  real fuzz = 0.0;

  constexpr explicit metal(const colour &a, const real f)
      : albedo(a), fuzz(std::clamp<real>(f, 0.0, 1.0)) {}
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
  colour albedo = colour(1.0);
  real index_of_refraction = 1.0;

  constexpr explicit dielectric(const colour &a, const real index_of_refraction)
      : albedo(a), index_of_refraction(index_of_refraction) {}
  virtual ~dielectric() = default;

  static inline real reflectance(const real cosine, const real index_ratio) {
    const real sqrt_r0 = (1.0 - index_ratio) / (1.0 + index_ratio);
    const real r0 = sqrt_r0 * sqrt_r0;
    return r0 + (1 - r0) * pow(1 - cosine, 5);
  }

  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       colour &attenuation, ray &scattered) const override {
    attenuation = albedo;
    const real index_ratio =
        rec.front_face ? 1.0 / index_of_refraction : index_of_refraction;

    const vec3 unit_direction = glm::normalize(r_in.dir);
    const real cos_theta =
        std::min<real>(-glm::dot(unit_direction, rec.normal), 1.0);
    const real sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    const bool cannot_reflect = index_ratio * sin_theta > 1.0;
    if (cannot_reflect ||
        util::random_real() < reflectance(cos_theta, index_ratio)) {
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
  std::shared_ptr<texture> emit;

  explicit diffuse_light(const std::shared_ptr<texture> &a) : emit(a) {}
  explicit diffuse_light(const colour &a)
      : emit(std::make_shared<solid_colour>(a)) {}

  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       colour &attenuation, ray &scattered) const override {
    return false;
  }

  virtual colour emitted(const real u, const real v,
                         const point3 &p) const override {
    return emit->value(u, v, p);
  }
};

/*
Stores a Wavefront MTL file
- Ns 10.000000                     - Specular exponent (currently ignored)
- Kd 1.000000 1.000000 1.000000    - Diffuse colour
- Ks 0.000000 0.000000 0.000000    - Specular colour
- Ke 0.000000 0.000000 0.000000    - Emissive colour
- Ni 1.500000                      - IOR
- map_Bump                         - Bump map (not yet supported)
- map_Kd                           - Diffuse map
- map_Ks                           - Specular map
*/
struct obj_material : public material {
  real specular_exponent = 0.0;
  real index_of_refraction = 0.0;
  real transparency = 0.0;
  colour ambient_colour = colour(0.0);
  colour diffuse_colour = colour(0.0);
  colour specular_colour = colour(0.0);
  colour emissive_colour = colour(0.0);

  std::shared_ptr<image_texture> ambient_map = nullptr;
  std::shared_ptr<image_texture> diffuse_map = nullptr;
  std::shared_ptr<image_texture> specular_map = nullptr;
  std::shared_ptr<image_texture> emissive_map = nullptr;
  std::shared_ptr<image_texture> bump_map = nullptr;

  explicit obj_material() = default;

  virtual bool scatter(const ray &r_in, const hit_record &rec,
                       colour &attenuation, ray &scattered) const override {

    const vec3 scatter_direction = rec.normal + util::random_unit_vector();
    if (util::near_zero(scatter_direction))
      return false;

    scattered = ray(rec.p, scatter_direction, r_in.time);
    if (diffuse_map != nullptr)
      attenuation = diffuse_colour * diffuse_map->value(rec.u, rec.v, rec.p);
    else
      attenuation = diffuse_colour;
    return true;
  }

  virtual colour emitted(const real u, const real v,
                         const point3 &p) const override {
    if (emissive_map != nullptr) {
      return emissive_colour * emissive_map->value(u, v, p);
    } else {
      return emissive_colour;
    }
  }
};
