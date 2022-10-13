
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
#include "transformed_instance.hpp"
#include "triangle.hpp"

#include "scene.hpp"

inline auto just_goose_scene() {
  // Image
  const real aspect_ratio = 1.0;
  const int image_width = 600;
  const int image_height = image_width / aspect_ratio;

  hittable_list world;

  const auto white_material =
      material_manager::create<lambertian>(colour(0.5, 0.5, 0.5));

  // const auto goose_texture = material_manager::create<lambertian>(
  //     std::make_shared<image_texture>("res/obj/goose/diffuse_1.png"));
  world.add(load_obj("res/obj/goose/goose.obj", white_material));

  auto list = hittable_list(std::make_shared<bvh>(world, 0.0, 1.0));
  list.add_background_map("res/hdr_pack/5.hdr");

  // Camera
  const point3 lookfrom(50, 50, 50);
  const point3 lookat(0, 20, 0);
  const vec3 up(0, 1, 0);
  const real dist_to_focus = glm::length(lookfrom - lookat);
  const real aperture = 0.1;

  const camera cam(image_width, image_height, lookfrom, lookat, up, 50,
                   aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

  return scene(list, cam);
}
