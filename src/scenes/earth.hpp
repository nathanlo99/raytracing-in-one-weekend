
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

auto earth() {
  const auto earth_texture = make_shared<image_texture>("../res/earthmap.jpg");
  const auto earth_surface =
      material_manager::instance().create<lambertian>(earth_texture);
  const auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

  return hittable_list(globe);
}
