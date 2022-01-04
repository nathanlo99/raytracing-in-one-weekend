
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
           const size_t end, const double time0, const double time1);
  virtual ~bvh_node() = default;

  static shared_ptr<bvh_node> from_list(const hittable_list &list,
                                        const double time0, const double time1);

  virtual bool hit(const ray &r, const double t_min, const double t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(const double time0, const double time1,
                            aabb &output_box) const override {
    output_box = box;
    return true;
  }
};

bool bvh_node::hit(const ray &r, const double t_min, const double t_max,
                   hit_record &rec) const {
  if (!box.hit(r, t_min, t_max))
    return false;

  const bool hit_left = left->hit(r, t_min, t_max, rec);
  const bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

  return hit_left || hit_right;
}

shared_ptr<bvh_node> bvh_node::from_list(const hittable_list &list,
                                         const double time0,
                                         const double time1) {
  std::vector<hittable_with_box> objects;
  objects.reserve(list.objects.size());
  std::transform(
      list.objects.begin(), list.objects.end(), std::back_inserter(objects),
      [time0, time1](const shared_ptr<hittable> &obj) -> hittable_with_box {
        aabb box;
        if (!obj->bounding_box(time0, time1, box)) {
          std::cerr << "Cannot create BVH from unbounded object" << std::endl;
        }
        return {obj, box};
      });
  return make_shared<bvh_node>(objects, 0, objects.size(), time0, time1);
}

bvh_node::bvh_node(std::vector<hittable_with_box> &objects, const size_t start,
                   const size_t end, const double time0, const double time1) {

  const size_t span = end - start;
  assert(span >= 2);
  if (span == 2) {
    left = objects[start].object;
    right = objects[start + 1].object;
    box = surrounding_box(objects[start].box, objects[start + 1].box);
    return;
  } else if (span == 3) {
    left = make_shared<bvh_node>(objects, start, end - 1, time0, time1);
    right = objects[end - 1].object;
    box = surrounding_box(
        objects[start].box,
        surrounding_box(objects[start + 1].box, objects[start + 2].box));
    return;
  }

  // Compute the widest span, and choose the axis appropriately
  aabb total_box;
  for (size_t i = start; i < end; ++i)
    total_box = surrounding_box(total_box, objects[i].box);
  const vec3 box_span = total_box.max - total_box.min;
  const int axis = box_span.largest_axis();

  std::sort(objects.begin() + start, objects.begin() + end,
            [axis](const hittable_with_box &a, const hittable_with_box &b) {
              return a.box.min[axis] < b.box.min[axis];
            });
  const size_t mid = start + span / 2;
  left = make_shared<bvh_node>(objects, start, mid, time0, time1);
  right = make_shared<bvh_node>(objects, mid, end, time0, time1);
  box = total_box;
}
