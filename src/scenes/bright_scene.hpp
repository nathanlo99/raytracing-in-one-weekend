
#pragma once

#include "util.hpp"

#include "animated_sphere.hpp"
#include "bvh_node.hpp"
#include "camera.hpp"
#include "colour.hpp"
#include "hittable_list.hpp"
#include "image.hpp"
#include "material.hpp"
#include "sphere.hpp"

#include "material_manager.hpp"

auto bright_scene() {
  // Image
  const double aspect_ratio = 16.0 / 9.0;
  const int image_width = 1200;
  const int image_height = static_cast<int>(image_width / aspect_ratio);

  hittable_list world;

  const auto ground_material =
      material_manager::instance().create<lambertian>(colour(0.5, 0.5, 0.5));
  world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

  const auto earth_texture = make_shared<image_texture>("../res/earthmap.jpg");

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      const double choose_mat = random_double();
      const double ball_height = 0.2;
      const point3 center(a + 0.9 * random_double(), ball_height,
                          b + 0.9 * random_double());

      if ((center - point3(4, 0.2, 0)).length() > 0.9) {
        if (choose_mat < 0.1) {
          // diffuse earth
          const auto sphere_material =
              material_manager::instance().create<lambertian>(earth_texture);
          world.add(make_shared<sphere>(center, 0.2, sphere_material));
        } else if (choose_mat < 0.4) {
          // diffuse
          const colour albedo = colour::random() * colour::random();
          const auto sphere_material =
              material_manager::instance().create<lambertian>(albedo);
          const point3 center2 = center + vec3(0, random_double(0, .5), 0);
          world.add(make_shared<animated_sphere>(center, center2, 0.0, 1.0, 0.2,
                                                 sphere_material));
        } else if (choose_mat < 0.8) {
          // diffuse
          const colour albedo = colour::random() * colour::random();
          const auto sphere_material =
              material_manager::instance().create<lambertian>(albedo);
          const point3 center2 = center + vec3(0, random_double(0, .5), 0);
          world.add(make_shared<animated_sphere>(center, center2, 0.0, 1.0, 0.2,
                                                 sphere_material));
        } else if (choose_mat < 0.95) {
          // metal
          const colour albedo = colour::random(0.5, 1);
          const double fuzz = random_double(0, 0.5);
          const auto sphere_material =
              material_manager::instance().create<metal>(albedo, fuzz);
          world.add(make_shared<sphere>(center, 0.2, sphere_material));
        } else {
          // glass
          const auto sphere_material =
              material_manager::instance().create<dielectric>(colour(1.0), 1.5);
          world.add(make_shared<sphere>(center, 0.2, sphere_material));
        }
      }
    }
  }

  const auto material1 =
      material_manager::instance().create<dielectric>(colour(1.0), 1.5);
  world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

  const auto material2 =
      material_manager::instance().create<lambertian>(colour(0.4, 0.2, 0.1));
  world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

  const auto material3 =
      material_manager::instance().create<metal>(colour(0.7, 0.6, 0.5), 0.0);
  world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

  auto list = hittable_list(bvh_node::from_list(world, 0.0, 1.0));

  const auto skybox_image = make_shared<image_texture>("../res/hdr_pack/5.hdr");
  const auto skybox_texture =
      material_manager::instance().create<diffuse_light>(skybox_image);
  list.add(make_shared<sphere>(point3(0, 0, 0), 9000, skybox_texture));

  // Camera
  const point3 lookfrom(13, 2, 3);
  const point3 lookat(0, 0, 0);
  const vec3 up(0, 1, 0);
  const double dist_to_focus = 10.0;
  const double aperture = 0.1;

  const camera cam(lookfrom, lookat, up, 20, aspect_ratio, aperture,
                   dist_to_focus, 0.0, 1.0);

  return std::make_tuple(list, cam, image_width, image_height);
}
