

#pragma once

#include "util.hpp"

#include "animated_sphere.hpp"
#include "bvh_node.hpp"
#include "camera.hpp"
#include "colour.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "material_manager.hpp"
#include "sphere.hpp"
#include "triangle.hpp"

#include "scene.hpp"

inline auto glass_test_scene() {
  const real aspect_ratio = 1.5;
  const int image_width = 1200;
  const int image_height = image_width / aspect_ratio;

  hittable_list world;

  const std::vector<real> iors = {1.0, 1.16, 1.33, 1.5, 1.66, 1.85, 2.0};
  for (int i = 0; i < static_cast<int>(iors.size()); ++i) {
    const real ior = iors[i];
    const auto white_glass_material =
        material_manager::create<dielectric>(colour(1.0), ior);
    const auto red_glass_material =
        material_manager::create<dielectric>(colour(1.0, 0.5, 0.5), ior);
    const auto blue_glass_material =
        material_manager::create<dielectric>(colour(0.5, 0.5, 1.0), ior);
    world.emplace_back<sphere>(vec3(i - 3, 0.5, 0), 0.5, white_glass_material);
    world.emplace_back<sphere>(vec3(i - 3, 1.5, 0), 0.5, red_glass_material);
    world.emplace_back<sphere>(vec3(i - 3, -0.5, 0), 0.5, blue_glass_material);
  }

  auto list = hittable_list(bvh_node::from_list(world, 0.0, 1.0));
  list.add_background_map("res/hdr_pack/5.hdr");

  // Camera
  const point3 lookfrom(0, 0.5, 5);
  const point3 lookat(0, 0.5, 0);
  const vec3 up(0, 1, 0);
  const real dist_to_focus = glm::length(lookfrom - lookat);
  const real aperture = 0.1;

  const camera cam(image_width, image_height, lookfrom, lookat, up, 50,
                   aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

  return scene(list, cam);
}
