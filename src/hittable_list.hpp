
#pragma once

#include "hittable.hpp"

#include <memory>
#include <vector>

using std::make_shared;
using std::shared_ptr;

struct hittable_list : public hittable {
  std::vector<shared_ptr<hittable>> objects;

  hittable_list() = default;
  hittable_list(shared_ptr<hittable> object) { add(object); }

  void clear() { objects.clear(); }
  void add(shared_ptr<hittable> object) { objects.push_back(object); }

  virtual bool hit(const ray &r, const double t_min, const double t_max,
                   hit_record &rec) const override;
};

bool hittable_list::hit(const ray &r, const double t_min, const double t_max,
                        hit_record &rec) const {
  hit_record temp_rec;
  bool hit_anything = false;
  double closest_so_far = t_max;

  for (const auto &object : objects) {
    if (object->hit(r, t_min, closest_so_far, temp_rec)) {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      rec = temp_rec;
    }
  }

  return hit_anything;
}
