

#pragma once

#include "util.hpp"

#include "animated_sphere.hpp"
#include "camera.hpp"
#include "colour.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "material_manager.hpp"
#include "obj_loader.hpp"
#include "sphere.hpp"
#include "transformed_hittable.hpp"
#include "triangle.hpp"

#include "scene.hpp"

inline auto instance_scene() {
  // Image
  const real aspect_ratio = 2.0;
  const int image_width = 1200;
  const int image_height = image_width / aspect_ratio;

  hittable_list world;

  const auto ground_material =
      material_manager::create<lambertian>(colour(0.03, 0.10, 0.03));
  world.emplace_back<sphere>(point3(0, -1000, 0), 1000, ground_material);

  const auto goose_obj = load_obj("res/obj/goose/goose.obj");
  const int spread = 30;
  const int spacing = 3;
  for (int i = -spread; i <= spread; i += spacing) {
    for (int j = -spread; j <= spread; j += spacing) {
      const real x = i + util::random_real(-0.5, 0.5);
      const real y = 0.0;
      const real z = j + util::random_real(-0.5, 0.5);
      const real angle = util::random_real(0.0, 360.0);
      const mat4 m = glm::translate(mat4(1.0), vec3(x, y, z)) *
                     glm::scale(mat4(1.0), vec3(0.06)) *
                     glm::rotate(mat4(1.0), util::degrees_to_radians(angle),
                                 vec3(0, 1, 0));
      world.emplace_back<transformed_hittable>(goose_obj, m);
    }
  }

  auto list = hittable_list(std::make_shared<bvh<>>(world, 0.0, 1.0));
  list.add_background_map("res/hdr_pack/5.hdr");

  // Camera
  const point3 lookfrom(0, 3, 6);
  const point3 lookat(0, 1, 0);
  const vec3 up(0, 1, 0);
  const real dist_to_focus = glm::length(lookfrom - lookat);
  const real aperture = 0.1;

  const camera cam(image_width, image_height, lookfrom, lookat, up, 50,
                   aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

  return scene(list, cam);
}
