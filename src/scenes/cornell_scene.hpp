
#pragma once

#include "transformed_hittable.hpp"
#include "util.hpp"

#include "animated_sphere.hpp"
#include "box.hpp"
#include "bvh.hpp"
#include "camera.hpp"
#include "colour.hpp"
#include "hittable_list.hpp"
#include "material.hpp"
#include "material_manager.hpp"
#include "sphere.hpp"
#include "triangle.hpp"

#include "scene.hpp"
#include <algorithm>

inline void add_xy_rect(hittable_list &lst, const real x0, const real x1,
                        const real y0, const real y1, const real z,
                        material *mat) {
  // p0 is the bottom left, proceeding to number counter-clockwise
  const point3 p0(x0, y0, z), p1(x1, y0, z), p2(x1, y1, z), p3(x0, y1, z);
  lst.emplace_back<triangle>(p0, p1, p2, mat);
  lst.emplace_back<triangle>(p0, p2, p3, mat);
}

inline void add_xz_rect(hittable_list &lst, const real x0, const real x1,
                        const real z0, const real z1, const real y,
                        material *mat) {
  // p0 is the bottom left, proceeding to number counter-clockwise
  const point3 p0(x0, y, z0), p1(x1, y, z0), p2(x1, y, z1), p3(x0, y, z1);
  lst.emplace_back<triangle>(p0, p1, p2, mat);
  lst.emplace_back<triangle>(p0, p2, p3, mat);
}

inline void add_yz_rect(hittable_list &lst, const real y0, const real y1,
                        const real z0, const real z1, const real x,
                        material *mat) {
  // p0 is the bottom left, proceeding to number counter-clockwise
  const point3 p0(x, y0, z0), p1(x, y1, z0), p2(x, y1, z1), p3(x, y0, z1);
  lst.emplace_back<triangle>(p0, p1, p2, mat);
  lst.emplace_back<triangle>(p0, p2, p3, mat);
}

inline void add_box(hittable_list &lst, const point3 &min, const point3 &max,
                    const real rotate_angle, material *mat) {
  const vec3 scale = max - min, translation = min;

  const mat4 identity = mat4(1.0);
  const mat4 m = glm::translate(identity, translation) *
                 glm::scale(identity, scale) *
                 glm::rotate(identity, util::degrees_to_radians(rotate_angle),
                             vec3(0.0, 1.0, 0.0));

  lst.emplace_back<transformed_hittable>(std::make_shared<box>(mat), m);
}

inline hittable_list cornell_box_objects() {
  hittable_list objects;

  const auto red =
      material_manager::create<lambertian>(colour(0.65, 0.05, 0.05));
  const auto white =
      material_manager::create<lambertian>(colour(0.73, 0.73, 0.73));
  const auto green =
      material_manager::create<lambertian>(colour(0.12, 0.45, 0.15));
  const auto light =
      material_manager::create<diffuse_light>(colour(25.0, 25.0, 12.5));

  add_yz_rect(objects, 0, 555, 0, 555, 555, red);
  add_yz_rect(objects, 0, 555, 0, 555, 0, green);
  add_xz_rect(objects, 213, 343, 227, 332, 554, light);
  add_xz_rect(objects, 0, 555, 0, 555, 0, white);
  add_xz_rect(objects, 0, 555, 0, 555, 555, white);
  add_xy_rect(objects, 0, 555, 0, 555, 555, white);

  add_box(objects, point3(130, 0, 65), point3(295, 165, 230), -15.0, white);
  add_box(objects, point3(265, 0, 295), point3(430, 330, 460), 18.0, white);

  return objects;
}

inline auto cornell_scene() {
  const real aspect_ratio = 1.0;
  const int image_width = 600;
  const int image_height = image_width / aspect_ratio;

  const auto objects = cornell_box_objects();

  // Camera
  const point3 lookfrom(278, 278, -800);
  const point3 lookat(278, 278, 0);
  const vec3 up(0.0, 1.0, 0.0);
  const real dist_to_focus = glm::length(lookfrom - lookat);
  const real aperture = 0.1;
  const real vfov = 40.0;

  const camera cam(image_width, image_height, lookfrom, lookat, up, vfov,
                   aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

  return scene(objects, cam);
}
