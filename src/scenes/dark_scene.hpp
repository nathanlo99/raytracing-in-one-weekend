
#pragma once

#include "util.hpp"

#include "animated_sphere.hpp"
#include "bvh.hpp"
#include "camera.hpp"
#include "colour.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "material_manager.hpp"
#include "sphere.hpp"

inline auto dark_scene() {
  hittable_list world;

  const auto ground_material =
      material_manager::create<lambertian>(colour(0.5, 0.5, 0.5));
  world.emplace_back<sphere>(point3(0, -1000, 0), 1000, ground_material);

  const auto earth_texture =
      std::make_shared<image_texture>("res/earthmap.jpg");

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      const real choose_mat = util::random_real();
      const real ball_height = 0.2;
      const point3 center(a + 0.9 * util::random_real(), ball_height,
                          b + 0.9 * util::random_real());

      if (length(center - point3(4, 0.2, 0)) > 0.9) {
        if (choose_mat < 0.1) {
          // diffuse
          const auto sphere_material =
              material_manager::create<diffuse_light>(earth_texture);
          world.emplace_back<sphere>(center, 0.2, sphere_material);
        } else if (choose_mat < 0.4) {
          // diffuse
          const colour albedo = util::random_vec3() * util::random_vec3();
          const auto sphere_material =
              material_manager::create<diffuse_light>(albedo);
          const point3 center2 = center + vec3(0, util::random_real(0, .5), 0);
          world.emplace_back<animated_sphere>(center, center2, 0.0, 1.0, 0.2,
                                              sphere_material);
        } else if (choose_mat < 0.8) {
          // diffuse
          const colour albedo = util::random_vec3() * util::random_vec3();
          const auto sphere_material =
              material_manager::create<lambertian>(albedo);
          const point3 center2 = center + vec3(0, util::random_real(0, 0.5), 0);
          world.emplace_back<animated_sphere>(center, center2, 0.0, 1.0, 0.2,
                                              sphere_material);
        } else if (choose_mat < 0.95) {
          // metal
          const colour albedo = util::random_vec3(0.5, 1);
          const real fuzz = util::random_real(0, 0.5);
          const auto sphere_material =
              material_manager::create<metal>(albedo, fuzz);
          world.emplace_back<sphere>(center, 0.2, sphere_material);
        } else {
          // glass
          const auto sphere_material =
              material_manager::create<dielectric>(colour(1.0), 1.5);
          world.emplace_back<sphere>(center, 0.2, sphere_material);
        }
      }
    }
  }

  const auto material1 = material_manager::create<dielectric>(colour(1.0), 1.5);
  world.emplace_back<sphere>(point3(0, 1, 0), 1.0, material1);

  const auto material2 =
      material_manager::create<diffuse_light>(colour(0.4, 0.2, 0.1));
  world.emplace_back<sphere>(point3(-4, 1, 0), 1.0, material2);

  const auto material3 =
      material_manager::create<metal>(colour(0.7, 0.6, 0.5), 0.0);
  world.emplace_back<sphere>(point3(4, 1, 0), 1.0, material3);

  return hittable_list(std::make_shared<bvh>(world, 0.0, 1.0));
}
