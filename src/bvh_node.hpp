
#pragma once

#include <algorithm>

#include "util.hpp"

#include "hittable.hpp"
#include "hittable_list.hpp"

struct hittable_with_box {
  shared_ptr<hittable> object;
  aabb box;
};

struct bvh_node : public hittable {
  shared_ptr<hittable> left, right;
  aabb box;

  bvh_node(std::vector<hittable_with_box> &src_objects, const size_t start,
           const size_t end);
  virtual ~bvh_node() = default;

  static shared_ptr<hittable> from_list(const hittable_list &list);

  virtual bool hit(const ray &r, const float t_min, const float t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(aabb &output_box) const override {
    output_box = box;
    return true;
  }
};
