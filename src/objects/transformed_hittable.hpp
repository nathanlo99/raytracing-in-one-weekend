
#pragma once

#include "hittable.hpp"
#include "util.hpp"

struct transformed_hittable : public hittable {
  std::shared_ptr<hittable> m_instance;
  mat4 m_model_matrix = mat4(1.0);
  mat4 m_inv_matrix = mat4(1.0);
  mat4 m_inv_trans_matrix = mat4(1.0);

public:
  transformed_hittable(const std::shared_ptr<hittable> &instance)
      : m_instance(instance), m_model_matrix(1.0), m_inv_matrix(1.0),
        m_inv_trans_matrix(1.0) {}
  transformed_hittable(const std::shared_ptr<hittable> &instance,
                       const mat4 &model_matrix)
      : m_instance(instance), m_model_matrix(model_matrix),
        m_inv_matrix(glm::inverse(model_matrix)),
        m_inv_trans_matrix(glm::transpose(m_inv_matrix)) {}
  virtual ~transformed_hittable() {}

  virtual bool hit(const ray &r, const real t_min, const real t_max,
                   hit_record &rec) const override {
    // 1. Transform ray with the inverse model matrix
    const vec3 new_origin = m_inv_matrix * vec4(r.orig, 1.0);
    const vec3 new_direction = m_inv_matrix * vec4(r.dir, 0.0);
    const ray new_ray(new_origin, new_direction, r.time);

    // 2. Hit the instance with that transformed_ray
    // 3. If missed, return false
    if (!m_instance->hit(new_ray, t_min, t_max, rec))
      return false;

    // 4. Transform everything in the hit_record accordingly and return true
    rec.p = m_model_matrix * vec4(rec.p, 1.0);
    rec.set_face_normal(r, m_inv_trans_matrix * vec4(rec.normal, 0.0));

    return true;
  }

  virtual bool bounding_box(const real time0, const real time1,
                            aabb &output_box) const override {
    if (!m_instance->bounding_box(time0, time1, output_box))
      return false;
    output_box = output_box.apply(m_model_matrix);
    return true;
  }
};