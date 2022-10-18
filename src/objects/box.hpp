
#pragma once

#include "hittable_list.hpp"
#include "material.hpp"
#include "quad.hpp"
#include "triangle.hpp"
#include "util.hpp"

struct box : public hittable {
  aabb m_bounding_box;
  hittable_list m_objects;

  box(material *mat) : box(point3(0.0), point3(1.0), mat) {}
  box(const point3 &p0, const point3 &p1, material *mat)
      : m_bounding_box(p0, p1) {
    const real x0 = p0.x, y0 = p0.y, z0 = p0.z;
    const real x1 = p1.x, y1 = p1.y, z1 = p1.z;
    const point3 a0(x0, y0, z0), a1(x1, y0, z0), a2(x1, y0, z1), a3(x0, y0, z1);
    const point3 b0(x0, y1, z0), b1(x1, y1, z0), b2(x1, y1, z1), b3(x0, y1, z1);
    // Bottom face
    m_objects.add(std::make_shared<quad>(a0, a1, a3, mat));
    // Top face
    m_objects.add(std::make_shared<quad>(b0, b1, b3, mat));
    // Front face
    m_objects.add(std::make_shared<quad>(a0, a1, b0, mat));
    // Back face
    m_objects.add(std::make_shared<quad>(a3, a2, b3, mat));
    // Right face
    m_objects.add(std::make_shared<quad>(a1, a2, b1, mat));
    // Left face
    m_objects.add(std::make_shared<quad>(a0, a3, b0, mat));
  }

  virtual ~box() {}

  virtual bool hit(const ray &r, const real t_min, const real t_max,
                   hit_record &rec) const override {
    return m_objects.hit(r, t_min, t_max, rec);
  }

  virtual bool bounding_box(const real time0, const real time1,
                            aabb &output_box) const override {
    output_box = m_bounding_box;
    return true;
  }
};