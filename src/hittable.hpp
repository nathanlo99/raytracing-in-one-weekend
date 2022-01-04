
#pragma once

#include "aabb.hpp"
#include "util.hpp"

struct material;

struct hit_record {
  point3 p;
  vec3 normal;
  shared_ptr<material> mat_ptr;
  double t;
  bool front_face;

  inline void set_face_normal(const ray &r, const vec3 &outward_normal) {
    front_face = dot(r.dir, outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};

class hittable {
public:
  // Return true if the specified ray hits the object between t_min and t_max,
  // with output variable rec, and false otherwise
  virtual bool hit(const ray &r, const double t_min, const double t_max,
                   hit_record &rec) const = 0;

  // Return true if the object has a bounding box across the entire region
  // [time0, time1], with output variable output_box, and false otherwise.
  virtual bool bounding_box(const double time0, const double time1,
                            aabb &output_box) const = 0;
};
