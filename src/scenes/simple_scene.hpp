
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

auto simple_scene() {
  hittable_list world;

  const auto ground_material = make_shared<lambertian>(colour(0.5, 0.5, 0.5));
  world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

  const auto material1 = make_shared<dielectric>(colour(1.0), 1.5);
  world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

  const auto material2 = make_shared<lambertian>(colour(0.4, 0.2, 0.1));
  world.add(make_shared<sphere>(point3(-2.5, 1, 0), 1.0, material2));

  const auto material3 = make_shared<metal>(colour(0.7, 0.6, 0.5), 0.0);
  world.add(make_shared<sphere>(point3(2.5, 1, 0), 1.0, material3));

  return hittable_list(bvh_node::from_list(world, 0.0, 1.0));
}
