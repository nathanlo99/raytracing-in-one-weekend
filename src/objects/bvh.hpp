
#pragma once

#include "hittable.hpp"
#include "hittable_list.hpp"
#include "util.hpp"

#include <stdexcept>
#include <vector>

struct bvh : public hittable {
  struct bvh_build_data {
    size_t primitive_index;
    aabb bounding_box;
    vec3 centroid;
  };

  struct bvh_entry {
    bool is_leaf = true;
    aabb bounding_box;
    size_t left_child = 0;
    size_t primitive_start = 0;
    size_t primitive_end = 0;
    size_t axis = 0;

    bvh_entry() = default;
  };

  std::vector<std::shared_ptr<hittable>> m_primitives;
  std::vector<bvh_entry> m_entries;

public:
  bvh(const hittable_list &lst, const real time0, const real time1,
      const size_t max_nodes_per_leaf = 16)
      : bvh(lst.m_objects, time0, time1, max_nodes_per_leaf) {}
  bvh(const std::vector<std::shared_ptr<hittable>> &objects, const real time0,
      const real time1, const size_t max_nodes_per_leaf = 16);

  virtual ~bvh() {}

  void recursive_build(std::vector<bvh_build_data> &data,
                       const size_t entry_idx, const size_t start,
                       const size_t end, const real time0, const real time1,
                       const size_t max_nodes_per_leaf);

  bool recursive_hit(const ray &r, const size_t idx, const real t_min,
                     const real t_max, hit_record &rec) const;

  virtual bool hit(const ray &r, const real t_min, const real t_max,
                   hit_record &rec) const override {
    return recursive_hit(r, 0, t_min, t_max, rec);
  }

  virtual bool bounding_box(const real time0, const real time1,
                            aabb &output_box) const override {
    output_box = m_entries[0].bounding_box;
    return true;
  }
};