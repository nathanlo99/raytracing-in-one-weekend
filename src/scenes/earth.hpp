
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

inline auto earth() {
  const auto earth_texture =
      std::make_shared<image_texture>("../res/earthmap.jpg");
  const auto earth_surface =
      material_manager::create<lambertian>(earth_texture);
  const auto globe = std::make_shared<sphere>(point3(0.0), 2.0, earth_surface);

  return hittable_list(globe);
}
