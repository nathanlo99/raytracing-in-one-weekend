
#pragma once

#include "hittable.hpp"
#include "util.hpp"

#include <memory>
#include <vector>

struct hittable_list : public hittable {
  std::vector<shared_ptr<hittable>> objects;

  hittable_list() = default;
  hittable_list(shared_ptr<hittable> object) { add(object); }

  void clear() { objects.clear(); }
  void add(shared_ptr<hittable> object) { objects.push_back(object); }

  virtual bool hit(const ray &r, const double t_min, const double t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(const double time0, const double time1,
                            aabb &output_box) const override;
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

bool hittable_list::bounding_box(const double time0, const double time1,
                                 aabb &output_box) const {
  if (objects.empty())
    return false;

  aabb temp_box;
  for (const auto &object : objects) {
    if (!object->bounding_box(time0, time1, temp_box))
      return false;
    output_box = surrounding_box(output_box, temp_box);
  }

  return true;
}
