
#pragma once

#include "util.hpp"

struct aabb {
  point3 minimum, maximum;

  aabb(const point3& min, const point3& max): minimum(min), maximum(max) {}
  
};
