
#pragma once

#include "util.hpp"

#include "animated_sphere.hpp"
#include "bvh_node.hpp"
#include "camera.hpp"
#include "colour.hpp"
#include "hittable_list.hpp"
#include "image.hpp"
#include "material.hpp"
#include "material_manager.hpp"
#include "mesh_loader.hpp"
#include "sphere.hpp"
#include "triangle.hpp"

#include "scene.hpp"

auto triangles_scene() {
  // Image
  const float aspect_ratio = 1.0;
  const int image_width = 800;
  const int image_height = static_cast<int>(image_width / aspect_ratio);

  hittable_list world;

  const auto ground_material =
      material_manager::instance().create<lambertian>(colour(0.5, 0.5, 0.5));
  world.add(make_shared<sphere>(point3(0, -1001, 0), 1000, ground_material));

  const auto glass_material = material_manager::instance().create<dielectric>(
      colour(0.2, 0.3, 0.8), 1.52);

  world.add(load_obj("../assets/obj/icosa.obj", glass_material));

  auto list = hittable_list(bvh_node::from_list(world, 0.0, 1.0));
  list.add_background_map("../res/hdr_pack/5.hdr");

  // Camera
  const point3 lookfrom(0, 1, 3);
  const point3 lookat(0, 0, 0);
  const vec3 up(0, 1, 0);
  const float dist_to_focus = length(lookfrom - lookat);
  const float aperture = 0.1;

  const camera cam(image_width, image_height, lookfrom, lookat, up, 50,
                   aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

  return scene(list, cam);
}
