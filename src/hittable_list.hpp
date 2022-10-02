
#pragma once

#include "hittable.hpp"
#include "util.hpp"

#include <memory>
#include <string>
#include <vector>

struct hittable_list : public hittable {
  std::vector<std::shared_ptr<hittable>> objects;

  hittable_list() = default;
  hittable_list(std::shared_ptr<hittable> object) { add(object); }

  constexpr size_t size() const { return objects.size(); }
  void clear() { objects.clear(); }
  void add(std::shared_ptr<hittable> object) { objects.emplace_back(object); }
  void add_background_map(const std::string &filename);

  virtual bool hit(const ray &r, const real t_min, const real t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(const real time0, const real time1,
                            aabb &output_box) const override;
};
