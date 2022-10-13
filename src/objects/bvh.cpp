

#include "bvh.hpp"

#include "aabb.hpp"
#include "hittable.hpp"
#include "util.hpp"

#include <vector>

bvh::bvh(const std::vector<std::shared_ptr<hittable>> &objects,
         const real time0, const real time1, const size_t max_nodes_per_leaf)
    : m_max_nodes_per_leaf(std::min<size_t>(max_nodes_per_leaf, 255)),
      m_primitives(), m_entries() {

  const auto start_ns = util::get_time_ns();

  std::vector<bvh_build_data> data(objects.size());
  aabb total_bounding_box;
  for (size_t i = 0; i < objects.size(); ++i) {
    const std::shared_ptr<hittable> &object = objects[i];
    aabb bounding_box;
    if (!object->bounding_box(time0, time1, bounding_box)) {
      throw std::runtime_error(
          std::string("Could not build BVH with unbounded hittable at index " +
                      std::to_string(i)));
    }
    data[i] = {i, bounding_box, (bounding_box.min + bounding_box.max) / 2.0};
    total_bounding_box.merge(bounding_box);
  }

  m_entries.emplace_back(); // Root
  recursive_build(data, 0, 0, data.size(), time0, time1);

  for (size_t entry_idx = 0; entry_idx < m_entries.size(); ++entry_idx) {
    bvh_entry &entry = m_entries[entry_idx];
    if (entry.is_leaf) {
      const size_t prim_start = entry.primitive_start,
                   prim_end = entry.primitive_end;
      entry.primitive_start = m_primitives.size();
      for (size_t data_idx = prim_start; data_idx < prim_end; ++data_idx) {
        m_primitives.push_back(objects[data[data_idx].primitive_index]);
      }
      entry.primitive_end = m_primitives.size();
    }
  }

  const auto end_ns = util::get_time_ns();
  const real total_seconds = (end_ns - start_ns) / 1e9;

  std::cout << "Finished constructing a BVH on a list of " << objects.size()
            << " items" << std::endl;
  std::cout << "  " << total_seconds << " seconds" << std::endl;
  std::cout << "  " << m_primitives.size() << " primitives" << std::endl;
  std::cout << "  " << m_entries.size() << " nodes" << std::endl;
}

void bvh::recursive_build(std::vector<bvh_build_data> &data,
                          const size_t entry_idx, const size_t start,
                          const size_t end, const real time0,
                          const real time1) {
  const size_t span = end - start;
  aabb total_bounding_box;
  for (size_t idx = start; idx < end; ++idx)
    total_bounding_box.merge(data[idx].bounding_box);

  if (span <= m_max_nodes_per_leaf) {
    m_entries[entry_idx] = {true, total_bounding_box, 0, start, end, 0};
    return;
  }

  // For now, just split like before
  const size_t axis = total_bounding_box.largest_axis();
  std::sort(data.begin() + start, data.begin() + end,
            [axis](const bvh_build_data &a, const bvh_build_data &b) {
              return a.centroid[axis] < b.centroid[axis];
            });
  const size_t mid = start + span / 2;

  const size_t left_entry_idx = m_entries.size(),
               right_entry_idx = left_entry_idx + 1;
  m_entries.emplace_back(); // Left
  m_entries.emplace_back(); // Right
  recursive_build(data, left_entry_idx, start, mid, time0, time1);
  recursive_build(data, right_entry_idx, mid, end, time0, time1);
  m_entries[entry_idx] = {false, total_bounding_box, left_entry_idx, 0, 0,
                          axis};
}

bool bvh::recursive_hit(const ray &r, const size_t idx, const real t_min,
                        const real t_max, hit_record &rec) const {
  const bvh_entry entry = m_entries[idx];
  if (!entry.bounding_box.does_hit(r, t_min, t_max))
    return false;

  if (entry.is_leaf) {
    bool hit_anything = false;
    const size_t prim_start = entry.primitive_start;
    const size_t prim_end = entry.primitive_end;
    float closest_so_far = t_max;
    for (size_t prim_idx = prim_start; prim_idx < prim_end; ++prim_idx) {
      const std::shared_ptr<hittable> &object = m_primitives[prim_idx];
      if (object->hit(r, t_min, closest_so_far, rec)) {
        hit_anything = true;
        closest_so_far = rec.t;
      }
    }
    return hit_anything;
  } else {
    const size_t left_idx = entry.left_child, right_idx = left_idx + 1;
    const bool increasing = r.dir[entry.axis] > 0;
    if (increasing) {
      const bool hit_left = recursive_hit(r, left_idx, t_min, t_max, rec);
      const bool hit_right =
          recursive_hit(r, right_idx, t_min, hit_left ? rec.t : t_max, rec);
      return hit_left | hit_right;
    } else {
      const bool hit_right = recursive_hit(r, right_idx, t_min, t_max, rec);
      const bool hit_left =
          recursive_hit(r, left_idx, t_min, hit_right ? rec.t : t_max, rec);
      return hit_left | hit_right;
    }
  }
}
