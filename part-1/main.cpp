
#include "util.hpp"

#include "camera.hpp"
#include "colour.hpp"
#include "hittable_list.hpp"
#include "image.hpp"
#include "material.hpp"
#include "sphere.hpp"

#include <fstream>
#include <iostream>

colour ray_colour(const ray &r, const hittable &world, int depth) {
  if (depth <= 0)
    return colour(0, 0, 0);

  hit_record rec;
  if (world.hit(r, eps, infinity, rec)) {
    ray scattered;
    colour attenuation;
    if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
      return attenuation * ray_colour(scattered, world, depth - 1);
    return colour(0, 0, 0);
  }
  const vec3 unit_direction = r.dir.normalized();
  const double t = (unit_direction.y() + 1.0) / 2.0;
  return (1.0 - t) * colour(1.0, 1.0, 1.0) + t * colour(0.5, 0.7, 1.0);
}

int main(int argc, char *argv[]) {
  // Image
  const double aspect_ratio = 16.0 / 9.0;
  const int image_width = 400;
  const int image_height = static_cast<int>(image_width / aspect_ratio);
  const int samples_per_pixel = 100;
  const int max_depth = 50;

  // World
  hittable_list world;
  const auto material_ground = make_shared<lambertian>(colour(0.8, 0.8, 0.0));
  const auto material_center = make_shared<lambertian>(colour(0.7, 0.3, 0.3));
  const auto material_left = make_shared<metal>(colour(0.8, 0.8, 0.8), 0.3);
  const auto material_right = make_shared<metal>(colour(0.8, 0.6, 0.2), 1.0);

  world.add(
      make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, material_ground));
  world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, material_center));
  world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, material_left));
  world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, material_right));
  // Camera
  camera cam;

  image image(image_width, image_height);
  for (int j = image_height - 1; j >= 0; --j) {
    std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
    for (int i = 0; i < image_width; ++i) {
      colour pixel_colour(0, 0, 0);
      for (int s = 0; s < samples_per_pixel; ++s) {
        const double u = (i + random_double()) / (image_width - 1);
        const double v = (j + random_double()) / (image_height - 1);
        const ray r = cam.get_ray(u, v);
        pixel_colour += ray_colour(r, world, max_depth);
      }
      image.set(j, i, pixel_colour / samples_per_pixel);
    }
  }
  std::string output_file = (argc > 1) ? argv[1] : "output.ppm";
  image.write_ppm(output_file);
  image.write_png("output.png");

  std::cerr << "\nDone.\n";
}
