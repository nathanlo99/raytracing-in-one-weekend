
#include "util.hpp"

#include "camera.hpp"
#include "colour.hpp"
#include "hittable_list.hpp"
#include "image.hpp"
#include "material.hpp"
#include "sphere.hpp"

#include <fstream>
#include <iomanip>
#include <iostream>

colour ray_colour(const ray &r, const hittable &world, const int depth) {
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

constexpr double compute_progress(const int image_width, const int image_height,
                                  const int j, const int i) {
  return static_cast<double>(j * image_width + i) /
         (image_width * image_height);
}

int main(int argc, char *argv[]) {
  // Image
  const double aspect_ratio = 16.0 / 9.0;
  const int image_width = 800;
  const int image_height = static_cast<int>(image_width / aspect_ratio);
  const int samples_per_pixel = 500;
  const int max_depth = 50;

  // World
  hittable_list world;
  const auto material_ground = lambertian(colour(0.8, 0.8, 0.0));
  const auto material_center = lambertian(colour(0.7, 0.3, 0.3));
  const auto material_left = metal(colour(0.8, 0.8, 0.8), 0.3);
  const auto material_right = metal(colour(0.8, 0.6, 0.2), 1.0);

  world.add(
      make_shared<sphere>(point3(0.0, -100.5, -1.0), 100.0, &material_ground));
  world.add(make_shared<sphere>(point3(0.0, 0.0, -1.0), 0.5, &material_center));
  world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, &material_left));
  world.add(make_shared<sphere>(point3(1.0, 0.0, -1.0), 0.5, &material_right));

  // Camera
  camera cam;

  double last_progress = 0.0, last_write = 0.0;
  image image(image_width, image_height);
  for (int j = 0; j < image_height; ++j) {
    for (int i = 0; i < image_width; ++i) {
      colour pixel_colour;
      for (int s = 0; s < samples_per_pixel; ++s) {
        const double u = (i + random_double()) / (image_width - 1);
        const double v = (j + random_double()) / (image_height - 1);
        const ray r = cam.get_ray(u, v);
        pixel_colour += ray_colour(r, world, max_depth);
      }
      image.set(j, i, pixel_colour / samples_per_pixel);

      const double progress = compute_progress(image_width, image_height, j, i);
      if (progress - last_progress > 0.0002) {
        std::cerr << "\rProgress: " << std::fixed << std::setprecision(2)
                  << (100 * progress) << "% " << std::flush;
        last_progress = progress;
      }
      if (progress - last_write > 0.01) {
        image.write_png("progress.png");
        last_write = progress;
      }
    }
  }
  std::cerr << "\rProgress: " << std::fixed << std::setprecision(2) << "100% "
            << std::flush;
  image.write_png("progress.png");
  std::cerr << "\nDone.\n";

  image.write_ppm("output.ppm");
  image.write_png("output.png");
}
