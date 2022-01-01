
#include "colour.hpp"
#include "ray.hpp"
#include "vec3.hpp"

#include <fstream>
#include <iostream>

colour ray_colour(const ray &r) {
  const vec3 unit_direction = unit_vector(r.dir);
  const double t = 0.5 * (unit_direction.y() + 1.0);
  return (1.0 - t) * colour(1.0, 1.0, 1.0) + t * colour(0.5, 0.7, 1.0);
}

int main(int argc, char *argv[]) {
  // Image
  const double aspect_ratio = 16.0 / 9.0;
  const int image_width = 400;
  const int image_height = static_cast<int>(image_width / aspect_ratio);

  // Camera
  const double viewport_height = 2.0;
  const double viewport_width = aspect_ratio * viewport_height;
  const double focal_length = 1.0;

  const point3 origin = point3(0, 0, 0);
  const vec3 horizontal = vec3(viewport_width, 0, 0);
  const vec3 vertical = vec3(0, viewport_height, 0);
  const vec3 lower_left_corner =
      origin - horizontal / 2.0 - vertical / 2.0 - vec3(0, 0, focal_length);

  std::string output_file = (argc > 1) ? argv[1] : "output.ppm";
  {
    std::ofstream out(output_file);
    out << "P3\n" << image_width << ' ' << image_height << "\n255\n";
    for (int j = image_height - 1; j >= 0; --j) {
      std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
      for (int i = 0; i < image_width; ++i) {
        const double u = double(i) / (image_width - 1);
        const double v = double(j) / (image_height - 1);

        const ray r(origin,
                    lower_left_corner + u * horizontal + v * vertical - origin);
        const colour pixel_colour = ray_colour(r);
        write_colour(out, pixel_colour);
      }
    }
  }
  std::cerr << "\nDone.\n";
}
