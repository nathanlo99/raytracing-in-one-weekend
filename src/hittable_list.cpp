
#include "hittable_list.hpp"

#include "material.hpp"
#include "material_manager.hpp"
#include "sphere.hpp"
#include "texture.hpp"

bool hittable_list::hit(const ray &r, const float t_min, const float t_max,
                        hit_record &rec) const {
  hit_record temp_rec;
  bool hit_anything = false;
  float closest_so_far = t_max;

  for (const auto &object : objects) {
    if (object->hit(r, t_min, closest_so_far, temp_rec)) {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      rec = temp_rec;
    }
  }

  return hit_anything;
}

bool hittable_list::bounding_box(aabb &output_box) const {
  if (objects.empty())
    return false;

  aabb temp_box;
  for (const auto &object : objects) {
    if (!object->bounding_box(temp_box))
      return false;
    output_box = surrounding_box(output_box, temp_box);
  }

  return true;
}

void hittable_list::add_background_map(const std::string &filename) {
  const auto radius = 90000.0;
  const auto skybox_image = make_shared<image_texture>(filename);
  const auto skybox_texture =
      material_manager::instance().create<diffuse_light>(skybox_image);
  add(make_shared<sphere>(point3(), radius, skybox_texture));
}
