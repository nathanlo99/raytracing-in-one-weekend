
#pragma once

#include "camera.hpp"
#include "hittable.hpp"

struct scene {
  hittable_list objects;
  camera cam;

  scene(const hittable_list &objects, const camera &cam)
      : objects(objects), cam(cam) {}
};
