
#pragma once

#include <algorithm>

#include "util.hpp"

#include "hittable.hpp"
#include "hittable_list.hpp"

struct hittable_with_box {
  std::shared_ptr<hittable> object;
  aabb box;
};

struct bvh_node : public hittable {
  std::shared_ptr<hittable> left, right;
  aabb box;

  bvh_node(std::vector<hittable_with_box> &src_objects, const size_t start,
           const size_t end, const real time0, const real time1);
  virtual ~bvh_node() = default;

  static std::shared_ptr<hittable>
  from_list(const hittable_list &list, const real time0, const real time1);

  virtual bool hit(const ray &r, const real t_min, const real t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(const real time0, const real time1,
                            aabb &output_box) const override {
    output_box = box;
    return true;
  }
};
