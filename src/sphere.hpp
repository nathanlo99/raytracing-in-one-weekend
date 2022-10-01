
#pragma once

#include "aabb.hpp"
#include "hittable.hpp"

struct sphere : public hittable {
  point3 centre;
  float radius;
  material *mat_ptr;

  sphere() : centre(), radius(0.0), mat_ptr(nullptr) {}
  sphere(const point3 &centre, const float r, material *mat)
      : centre(centre), radius(r), mat_ptr(mat) {}
  virtual ~sphere() {}

  virtual bool hit(const ray &r, const float t_min, const float t_max,
                   hit_record &rec) const override;
  virtual bool bounding_box(const float time0, const float time1,
                            aabb &output_box) const override;

  static constexpr void get_sphere_uv(const point3 &p, float &u, float &v) {
    const float theta = acos(-p.y);
    const float phi = atan2(-p.z, p.x) + pi;
    u = phi / (2.0 * pi);
    v = theta / pi;
  }
};
