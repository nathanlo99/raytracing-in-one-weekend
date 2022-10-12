
#include "bvh_node.hpp"

bool bvh_node::hit(const ray &r, const real t_min, const real t_max,
                   hit_record &rec) const {
  if (!m_box.does_hit(r, t_min, t_max))
    return false;

  const bool increasing = m_axis == -1 || r.dir[m_axis] > 0;

  if (increasing) {
    // NOTE: We still need to query both children because we care about the
    // record's hit location, and the distance to the right child may be smaller
    // than the distance to the left child, despite hitting both children
    const bool hit_left = m_left->hit(r, t_min, t_max, rec);
    const bool hit_right =
        m_right->hit(r, t_min, hit_left ? rec.t : t_max, rec);
    return hit_left | hit_right;
  } else {
    const bool hit_right = m_right->hit(r, t_min, t_max, rec);
    const bool hit_left = m_left->hit(r, t_min, hit_right ? rec.t : t_max, rec);
    return hit_left | hit_right;
  }
}

std::shared_ptr<hittable> bvh_node::from_list(const hittable_list &list,
                                              const real time0,
                                              const real time1) {
  if (list.size() == 1)
    return list.m_objects[0];

  std::vector<hittable_with_box> objects;
  objects.reserve(list.m_objects.size());
  aabb tmp_box;
  for (const std::shared_ptr<hittable> &obj : list.m_objects) {
    if (!obj->bounding_box(time0, time1, tmp_box)) {
      std::cerr << "Cannot create BVH from unbounded object" << std::endl;
    }
    objects.push_back({obj, tmp_box});
  }
  return std::make_shared<bvh_node>(objects, 0, objects.size(), time0, time1);
}

bvh_node::bvh_node(std::vector<hittable_with_box> &objects, const size_t start,
                   const size_t end, const real time0, const real time1) {

  const size_t span = end - start;
  assert(span >= 2);
  if (span == 2) {
    m_left = objects[start].object;
    m_right = objects[start + 1].object;
    m_box = surrounding_box(objects[start].box, objects[start + 1].box);
    m_axis = -1;
    return;
  } else if (span == 3) {
    m_left =
        std::make_shared<bvh_node>(objects, start, start + 2, time0, time1);
    m_right = objects[start + 2].object;
    m_box = surrounding_box(
        objects[start].box,
        surrounding_box(objects[start + 1].box, objects[start + 2].box));
    m_axis = -1;
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
  m_left = std::make_shared<bvh_node>(objects, start, mid, time0, time1);
  m_right = std::make_shared<bvh_node>(objects, mid, end, time0, time1);
  m_axis = axis;
  m_box = total_box;
}
