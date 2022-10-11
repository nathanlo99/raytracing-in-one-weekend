
#pragma once

#include "util.hpp"

#include "bvh_node.hpp"
#include "camera.hpp"
#include "colour.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "material_manager.hpp"
#include "obj_loader.hpp"
#include "sphere.hpp"

#include "scene.hpp"

inline auto diamond_scene() {
  // Image
  const real aspect_ratio = 1.5;
  const int image_width = 1200;
  const int image_height = image_width / aspect_ratio;

  hittable_list world;

  const auto ground_material =
      material_manager::create<lambertian>(colour(0.5, 0.5, 0.5));
  world.emplace_back<sphere>(point3(0, -1000, 0), 1000, ground_material);

  const auto diamond_material =
      material_manager::create<dielectric>(colour(1.0), 2.418);

  world.add(load_obj("res/obj/diamond.obj", diamond_material));

  auto list = hittable_list(bvh_node::from_list(world, 0.0, 1.0));
  list.add_background_map("res/hdr_pack/5.hdr");

  // Camera
  const point3 lookfrom(-0.5, 0.8, 4.5);
  const point3 lookat(0, 0.7, 0);
  const vec3 up(0, 1, 0);
  const real dist_to_focus = glm::length(lookfrom - lookat);
  const real aperture = 0.1;

  const camera cam(image_width, image_height, lookfrom, lookat, up, 50,
                   aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

  return scene(list, cam);
}
