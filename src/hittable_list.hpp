
#pragma once

#include "hittable.hpp"
#include "util.hpp"

#include <memory>
#include <string>
#include <vector>

struct hittable_list : public hittable {
  std::vector<shared_ptr<hittable>> objects;

  hittable_list() = default;
  hittable_list(shared_ptr<hittable> object) { add(object); }

  constexpr size_t size() const { return objects.size(); }
  void clear() { objects.clear(); }
  void add(shared_ptr<hittable> object) { objects.push_back(object); }
  void add_background_map(const std::string &filename);

  virtual bool hit(const ray &r, const double t_min, const double t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(const double time0, const double time1,
                            aabb &output_box) const override;
};
