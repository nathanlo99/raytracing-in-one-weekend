
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

hittable_list random_scene() {
  hittable_list world;

  auto ground_material = make_shared<lambertian>(colour(0.5, 0.5, 0.5));
  world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      auto choose_mat = random_double();
      const double ball_height =
          0.2 + random_double(); // std::max(0.2, random_double() * 0.4);
      const point3 center(a + 0.9 * random_double(), ball_height,
                          b + 0.9 * random_double());

      if ((center - point3(4, 0.2, 0)).length() > 0.9) {
        if (choose_mat < 0.8) {
          // diffuse
          const auto albedo = colour::random() * colour::random();
          const shared_ptr<material> sphere_material =
              make_shared<lambertian>(albedo);
          world.add(make_shared<sphere>(center, 0.2, sphere_material));
        } else if (choose_mat < 0.95) {
          // metal
          const auto albedo = colour::random(0.5, 1);
          const auto fuzz = random_double(0, 0.5);
          const shared_ptr<material> sphere_material =
              make_shared<metal>(albedo, fuzz);
          world.add(make_shared<sphere>(center, 0.2, sphere_material));
        } else {
          // glass
          const shared_ptr<material> sphere_material =
              make_shared<dielectric>(colour(1.0), 1.5);
          world.add(make_shared<sphere>(center, 0.2, sphere_material));
        }
      }
    }
  }

  const auto material1 = make_shared<dielectric>(colour(1.0), 1.5);
  world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

  const auto material2 = make_shared<lambertian>(colour(0.4, 0.2, 0.1));
  world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

  const auto material3 = make_shared<metal>(colour(0.7, 0.6, 0.5), 0.0);
  world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

  return world;
}

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
  const vec3 unit_direction = r.dir.normalize();
  const double t = (unit_direction.y() + 1.0) / 2.0;
  return (1 - t) * colour(1.0) + t * colour(0.5, 0.7, 1.0);
}

constexpr double compute_progress(const int image_width, const int image_height,
                                  const int j, const int i) {
  return static_cast<double>(j * image_width + i) /
         (image_width * image_height);
}

int main(int argc, char *argv[]) {
  // Image
  const double aspect_ratio = 3.0 / 2.0;
  const int image_width = 400;
  const int image_height = static_cast<int>(image_width / aspect_ratio);
  const int samples_per_pixel = 1000;
  const int max_depth = 50;

  // World
  auto world = random_scene();

  // Camera
  const point3 lookfrom(13, 2, 3);
  const point3 lookat(0, 0, 0);
  const vec3 up(0, 1, 0);
  const double dist_to_focus = 10.0;
  const double aperture = 0.1;

  camera cam(lookfrom, lookat, up, 20, aspect_ratio, aperture, dist_to_focus);

  double last_progress = 0.0, last_write = 0.0;
  image image(image_width, image_height);
  std::vector<colour> buffer(image_width * image_height);
  const long long num_pixels = image_width * image_height;
  const long long num_samples = num_pixels * samples_per_pixel;
  std::cerr << "Starting render..." << std::endl;
  std::cerr << "Estimated " << num_samples << " samples" << std::endl;
  for (int s = 1; s <= samples_per_pixel; ++s) {
    for (int j = 0; j < image_height; ++j) {
      for (int i = 0; i < image_width; ++i) {
        const double u = (i + random_double()) / (image_width - 1);
        const double v = (j + random_double()) / (image_height - 1);
        const ray r = cam.get_ray(u, v);
        const int idx = j * image_width + i;
        buffer[idx] += ray_colour(r, world, max_depth);
        image.set(j, i, buffer[idx] / s);
      }
    }
    std::cerr << "\rDone pass " << s << "/" << samples_per_pixel << " "
              << std::flush;
    image.write_png("output/progress.png");
  }
  std::cout << std::endl << "Done!" << std::endl;

  const std::string output_file = (argc > 1) ? argv[1] : "output/output.png";
  image.write_png(output_file);
}
