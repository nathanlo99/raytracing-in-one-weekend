
#include "bvh_node.hpp"

bool bvh_node::hit(const ray &r, const real t_min, const real t_max,
                   hit_record &rec) const {
  if (!box.does_hit(r, t_min, t_max))
    return false;

  // NOTE: We still need to query both children because we care about the
  // record's hit location, and the distance to the right child may be smaller
  // than the distance to the left child, despite hitting both children
  const bool hit_left = left->hit(r, t_min, t_max, rec);
  const bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);
  return hit_left | hit_right;
}

std::shared_ptr<hittable> bvh_node::from_list(const hittable_list &list,
                                              const real time0,
                                              const real time1) {
  if (list.size() == 1)
    return list.objects[0];

  std::vector<hittable_with_box> objects;
  objects.reserve(list.objects.size());
  for (const std::shared_ptr<hittable> &obj : list.objects) {
    aabb box;
    if (!obj->bounding_box(time0, time1, box)) {
      std::cerr << "Cannot create BVH from unbounded object" << std::endl;
    }
    objects.push_back({obj, box});
  }
  return std::make_shared<bvh_node>(objects, 0, objects.size(), time0, time1);
}

bvh_node::bvh_node(std::vector<hittable_with_box> &objects, const size_t start,
                   const size_t end, const real time0, const real time1) {

  const size_t span = end - start;
  assert(span >= 2);
  if (span == 2) {
    left = objects[start].object;
    right = objects[start + 1].object;
    box = surrounding_box(objects[start].box, objects[start + 1].box);
    return;
  } else if (span == 3) {
    left = std::make_shared<bvh_node>(objects, start, start + 2, time0, time1);
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
  const size_t axis = util::largest_axis(box_span);

  std::sort(objects.begin() + start, objects.begin() + end,
            [axis](const hittable_with_box &a, const hittable_with_box &b) {
              return (a.box.min[axis] + a.box.max[axis]) / 2 <
                     (b.box.min[axis] + b.box.max[axis]) / 2;
            });

  const size_t mid = start + span / 2;
  left = std::make_shared<bvh_node>(objects, start, mid, time0, time1);
  right = std::make_shared<bvh_node>(objects, mid, end, time0, time1);
  box = total_box;
}
