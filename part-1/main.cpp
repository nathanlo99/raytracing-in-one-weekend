
#include "util.hpp"

#include "camera.hpp"
#include "colour.hpp"
#include "hittable_list.hpp"
#include "sphere.hpp"

#include <fstream>
#include <iostream>

colour ray_colour(const ray &r, const hittable &world) {
  hit_record rec;
  if (world.hit(r, 0, infinity, rec)) {
    return 0.5 * (rec.normal + colour(1, 1, 1));
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

  // World
  hittable_list world;
  world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
  world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));

  // Camera
  camera cam;

  std::string output_file = (argc > 1) ? argv[1] : "output.ppm";
  {
    std::ofstream out(output_file);
    out << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for (int j = image_height - 1; j >= 0; --j) {
      std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
      for (int i = 0; i < image_width; ++i) {
        colour pixel_colour(0, 0, 0);
        for (int s = 0; s < samples_per_pixel; ++s) {
          const double u = (i + random_double()) / (image_width - 1);
          const double v = (j + random_double()) / (image_height - 1);
          const ray r = cam.get_ray(u, v);
          pixel_colour += ray_colour(r, world);
        }
        write_colour(out, pixel_colour, samples_per_pixel);
      }
    }
  }
  std::cerr << "\nDone.\n";
}
