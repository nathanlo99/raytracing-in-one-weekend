
#pragma once

#include "hittable.hpp"
#include "util.hpp"

struct transformed_hittable : public hittable {
  std::shared_ptr<hittable> m_instance;
  mat4 m_model_matrix = mat4(1.0);
  mat4 m_inv_trans_matrix = mat4(1.0);

public:
  transformed_hittable(const std::shared_ptr<hittable> &instance)
      : m_instance(instance), m_model_matrix(1.0), m_inv_trans_matrix(1.0) {}
  transformed_hittable(const std::shared_ptr<hittable> &instance,
                       const mat4 &model_matrix)
      : m_instance(instance), m_model_matrix(model_matrix),
        m_inv_trans_matrix(glm::transpose(glm::inverse(model_matrix))) {}
  virtual ~transformed_hittable() {}

  virtual bool hit(const ray &r, const real t_min, const real t_max,
                   hit_record &rec) const override {
    /* TODO
    1. Transform ray
    2. Hit the instance with that transformed_ray
    3. If missed, return false
    4. If hit, transform everything in the hit_record accordingly and return
       true.
    */
    return false;
  }
  virtual bool bounding_box(const real time0, const real time1,
                            aabb &output_box) const override {
    /* TODO
    1. If the instance is unbounded (cached), return false
    2. If we have a bounding box cached, return it
    3. Otherwise, compute the bounding box by transforming the instance's
       bounding box, cache a yes answer, and return it
    */
    return false;
  }
};