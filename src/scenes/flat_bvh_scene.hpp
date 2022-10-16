
#pragma once

#include "util.hpp"

#include "animated_sphere.hpp"
#include "bvh.hpp"
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

inline auto flat_bvh_scene() {
  // Image
  const real aspect_ratio = 1.0;
  const int image_width = 600;
  const int image_height = image_width / aspect_ratio;

  hittable_list world;

  const auto white_material =
      material_manager::create<lambertian>(colour(0.5, 0.5, 0.5));
  world.add(load_obj("res/obj/smooth_cube.obj", white_material, false));

  auto list = hittable_list(std::make_shared<bvh<>>(world, 0.0, 1.0));
  list.add_background_map("res/hdr_pack/5.hdr");

  // Camera
  const point3 lookfrom(3, 4, 5);
  const point3 lookat(0.5, 0.5, 0.5);
  const vec3 up(0, 1, 0);
  const real dist_to_focus = glm::length(lookfrom - lookat);
  const real aperture = 0.1;

  const camera cam(image_width, image_height, lookfrom, lookat, up, 50,
                   aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

  return scene(list, cam);
}
