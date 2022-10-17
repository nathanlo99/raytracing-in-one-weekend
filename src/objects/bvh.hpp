
#pragma once

#include "hittable.hpp"
#include "hittable_list.hpp"
#include "util.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <vector>

enum bvh_split_strategy {
  EqualParts,
  HalveLongestAxis,
  SAH,
};

template <bvh_split_strategy split_strategy = SAH>
struct bvh : public hittable {
  struct bvh_build_data {
    size_t primitive_index;
    aabb bounding_box;
    vec3 centroid;
  };

  struct bvh_entry {
    bool is_leaf = true;
    aabb bounding_box;
    union {
      size_t left_child;
      size_t primitive_start;
    };
    union {
      size_t axis;
      size_t primitive_end;
    };

    bvh_entry() = default;
    void construct_non_leaf(const aabb &box, const size_t left,
                            const size_t split_axis) {
      is_leaf = false;
      bounding_box = box;
      left_child = left;
      axis = split_axis;
    }

    void construct_leaf(const aabb &box, const size_t start, const size_t end) {
      is_leaf = true;
      bounding_box = box;
      primitive_start = start;
      primitive_end = end;
    }
  };

  std::vector<std::shared_ptr<hittable>> m_primitives;
  std::vector<bvh_entry> m_entries;

public:
  bvh(const hittable_list &lst, const real time0, const real time1,
      const size_t max_nodes_per_leaf = 16)
      : bvh(lst.m_objects, time0, time1, max_nodes_per_leaf) {}
  bvh(const std::vector<std::shared_ptr<hittable>> &objects, const real time0,
      const real time1, const size_t max_nodes_per_leaf);

  virtual ~bvh() {}

  void recursive_build(std::vector<bvh_build_data> &data,
                       const size_t entry_idx, const size_t start,
                       const size_t end, const real time0, const real time1,
                       const size_t max_nodes_per_leaf);

  bool recursive_hit(const ray &r, const size_t idx, const real t_min,
                     const real t_max, hit_record &rec) const;

  virtual bool hit(const ray &r, const real t_min, const real t_max,
                   hit_record &rec) const override {
    if (m_entries.empty())
      return false;
    return recursive_hit(r, 0, t_min, t_max, rec);
  }

  virtual bool bounding_box(const real time0, const real time1,
                            aabb &output_box) const override {
    if (m_entries.empty())
      return false;
    output_box = m_entries[0].bounding_box;
    return true;
  }

private:
  std::pair<size_t, size_t> split(std::vector<bvh_build_data> &data,
                                  const size_t start, const size_t end,
                                  const aabb &total_bounding_box) const;
};

// ============================= IMPLEMENTATION =============================

template <bvh_split_strategy strategy>
bvh<strategy>::bvh(const std::vector<std::shared_ptr<hittable>> &objects,
                   const real time0, const real time1,
                   size_t max_nodes_per_leaf)
    : m_primitives(), m_entries() {
  max_nodes_per_leaf = std::min<size_t>(max_nodes_per_leaf, 255);
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
    data[i] = {i, bounding_box, bounding_box.centroid()};
    total_bounding_box.merge(bounding_box);
  }

  m_entries.emplace_back(); // Root
  recursive_build(data, 0, 0, data.size(), time0, time1, max_nodes_per_leaf);

  for (bvh_entry &entry : m_entries) {
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

template <>
inline std::pair<size_t, size_t>
bvh<HalveLongestAxis>::split(std::vector<bvh_build_data> &data,
                             const size_t start, const size_t end,
                             const aabb &total_bounding_box) const {
  const size_t axis = total_bounding_box.largest_axis();
  const auto &cmp_to_center = [axis,
                               total_bounding_box](const bvh_build_data &a) {
    return a.centroid[axis] < total_bounding_box.centroid()[axis];
  };
  const size_t mid =
      std::partition(data.begin() + start, data.begin() + end, cmp_to_center) -
      data.begin();
  if (mid != start && mid != end)
    return {mid, axis};

  // If the split above was trivial, fall back to splitting into an equal number
  // of primitives
  const size_t fallback_mid = start + (end - start) / 2;
  const auto &cmp = [axis](const bvh_build_data &a, const bvh_build_data &b) {
    return a.centroid[axis] < b.centroid[axis];
  };
  std::nth_element(data.begin() + start, data.begin() + fallback_mid,
                   data.begin() + end, cmp);
  return {fallback_mid, axis};
}

template <>
inline std::pair<size_t, size_t>
bvh<EqualParts>::split(std::vector<bvh_build_data> &data, const size_t start,
                       const size_t end, const aabb &total_bounding_box) const {
  const size_t axis = total_bounding_box.largest_axis();
  const auto &cmp = [axis](const bvh_build_data &a, const bvh_build_data &b) {
    return a.centroid[axis] < b.centroid[axis];
  };
  const size_t mid = start + (end - start) / 2;
  std::nth_element(data.begin() + start, data.begin() + mid, data.begin() + end,
                   cmp);
  return std::make_pair(mid, axis);
}

template <>
inline std::pair<size_t, size_t>
bvh<SAH>::split(std::vector<bvh_build_data> &data, const size_t start,
                const size_t end, const aabb &total_bounding_box) const {
  size_t best_mid = -1, best_axis = -1;
  real best_cost = inf;

  // 1. For each axis:
  for (size_t axis = 0; axis < 3; ++axis) {
    // a. Sort the build data with respect to centroids on the chosen axis
    const auto &cmp = [axis](const bvh_build_data &a, const bvh_build_data &b) {
      return a.centroid[axis] < b.centroid[axis];
    };
    std::sort(data.begin() + start, data.begin() + end, cmp);

    // prefix_boxes[idx] stores the bounding box around [start, start + idx)
    // suffix_boxes[idx] stores the bounding box around [start + idx, end)
    std::vector<aabb> prefix_boxes(end - start), suffix_boxes(end - start);
    aabb prefix_box, suffix_box;
    for (int idx = start + 1; idx < static_cast<int>(end); ++idx) {
      prefix_boxes[idx - start] = prefix_box;
      prefix_box.merge(data[idx].bounding_box);
    }

    for (int idx = end - 1; idx >= static_cast<int>(start); --idx) {
      suffix_box.merge(data[idx].bounding_box);
      suffix_boxes[idx - start] = suffix_box;
    }

    // b. For every non-trivial split,
    for (size_t mid = start + 1; mid < end - 1; ++mid) {
      const aabb bounding_box_A = prefix_boxes[mid - start];
      const aabb bounding_box_B = suffix_boxes[mid - start];

      // c. Compute the cost:
      //      0.125 + (A.size() * A.SA() + B.size() * B.SA()) / AB.SA()
      const size_t size_A = mid - start, size_B = end - mid;
      const real cost = 0.125 + (size_A * bounding_box_A.surface_area() +
                                 size_B * bounding_box_B.surface_area()) /
                                    total_bounding_box.surface_area();

      // d. Keep track of the best (mid, axis, cost) tuple
      if (cost < best_cost) {
        best_mid = mid;
        best_axis = axis;
        best_cost = cost;
      }
    }
  }

  // 2. Perform the split:
  // a. Use nth_element on the best axis and return mid
  const auto &cmp = [best_axis](const bvh_build_data &a,
                                const bvh_build_data &b) {
    return a.centroid[best_axis] < b.centroid[best_axis];
  };
  std::nth_element(data.begin() + start, data.begin() + best_mid,
                   data.begin() + end, cmp);

  std::cout << start << " - " << best_mid << " - " << end << std::endl;
  return std::make_pair(best_mid, best_axis);
}

template <bvh_split_strategy strategy>
void bvh<strategy>::recursive_build(std::vector<bvh_build_data> &data,
                                    const size_t entry_idx, const size_t start,
                                    const size_t end, const real time0,
                                    const real time1,
                                    const size_t max_nodes_per_leaf) {
  const size_t span = end - start;
  aabb total_bounding_box;
  for (size_t idx = start; idx < end; ++idx)
    total_bounding_box.merge(data[idx].bounding_box);

  if (span <= max_nodes_per_leaf) {
    m_entries[entry_idx].construct_leaf(total_bounding_box, start, end);
    return;
  }

  const auto &[mid, axis] = split(data, start, end, total_bounding_box);

  const size_t left_entry_idx = m_entries.size(),
               right_entry_idx = left_entry_idx + 1;
  m_entries.emplace_back(); // Left
  m_entries.emplace_back(); // Right
  recursive_build(data, left_entry_idx, start, mid, time0, time1,
                  max_nodes_per_leaf);
  recursive_build(data, right_entry_idx, mid, end, time0, time1,
                  max_nodes_per_leaf);
  m_entries[entry_idx].construct_non_leaf(total_bounding_box, left_entry_idx,
                                          axis);
}

template <bvh_split_strategy strategy>
bool bvh<strategy>::recursive_hit(const ray &r, const size_t idx,
                                  const real t_min, const real t_max,
                                  hit_record &rec) const {
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
