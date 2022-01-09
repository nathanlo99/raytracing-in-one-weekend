
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

auto dark_scene() {
  hittable_list world;

  const auto ground_material = make_shared<lambertian>(colour(0.5, 0.5, 0.5));
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
          // diffuse
          const shared_ptr<material> sphere_material =
              make_shared<diffuse_light>(earth_texture);
          world.add(make_shared<sphere>(center, 0.2, sphere_material));
        } else if (choose_mat < 0.4) {
          // diffuse
          const colour albedo = colour::random() * colour::random();
          const shared_ptr<material> sphere_material =
              make_shared<diffuse_light>(albedo);
          const point3 center2 = center + vec3(0, random_double(0, .5), 0);
          world.add(make_shared<animated_sphere>(center, center2, 0.0, 1.0, 0.2,
                                                 sphere_material));
        } else if (choose_mat < 0.8) {
          // diffuse
          const colour albedo = colour::random() * colour::random();
          const shared_ptr<material> sphere_material =
              make_shared<lambertian>(albedo);
          const point3 center2 = center + vec3(0, random_double(0, .5), 0);
          world.add(make_shared<animated_sphere>(center, center2, 0.0, 1.0, 0.2,
                                                 sphere_material));
        } else if (choose_mat < 0.95) {
          // metal
          const colour albedo = colour::random(0.5, 1);
          const double fuzz = random_double(0, 0.5);
          const shared_ptr<material> sphere_material =
              make_shared<metal>(albedo, fuzz);
          world.add(make_shared<sphere>(center, 0.2, sphere_material));
        } else {
          // glass
          const shared_ptr<material> sphere_material =
              make_shared<dielectric>(colour(1.0), 1.5);
          world.add(make_shared<sphere>(center, 0.2, sphere_material));
        }
      }
    }
  }

  const auto material1 = make_shared<dielectric>(colour(1.0), 1.5);
  world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

  const auto material2 = make_shared<diffuse_light>(colour(0.4, 0.2, 0.1));
  world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

  const auto material3 = make_shared<metal>(colour(0.7, 0.6, 0.5), 0.0);
  world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

  return hittable_list(bvh_node::from_list(world, 0.0, 1.0));
}
