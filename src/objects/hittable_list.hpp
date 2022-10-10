
#pragma once

#include "hittable.hpp"
#include "util.hpp"

#include <memory>
#include <string>
#include <vector>

struct hittable_list : public hittable {
  std::vector<std::shared_ptr<hittable>> m_objects;

  hittable_list() = default;
  hittable_list(const std::shared_ptr<hittable> &object) { add(object); }

  constexpr size_t size() const { return m_objects.size(); }
  void clear() { m_objects.clear(); }
  void add(const std::shared_ptr<hittable> &object) {
    m_objects.emplace_back(object);
  }

  template <class T, class... Args> void emplace_back(Args &&...args) {
    m_objects.emplace_back(std::make_shared<T>(std::forward<Args>(args)...));
  }

  void add_background_map(const std::string_view &filename);

  virtual bool hit(const ray &r, const real t_min, const real t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(const real time0, const real time1,
                            aabb &output_box) const override;
};
