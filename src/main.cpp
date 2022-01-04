
#include "util.hpp"

#include "animated_sphere.hpp"
#include "bvh_node.hpp"
#include "camera.hpp"
#include "colour.hpp"
#include "hittable_list.hpp"
#include "image.hpp"
#include "material.hpp"
#include "sphere.hpp"

#include <atomic>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <queue>
#include <thread>

auto random_scene() {
  hittable_list world;

  const auto ground_material = make_shared<lambertian>(colour(0.5, 0.5, 0.5));
  world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

  for (int a = -11; a < 11; a++) {
    for (int b = -11; b < 11; b++) {
      const double choose_mat = random_double();
      const double ball_height = 0.2;
      const point3 center(a + 0.9 * random_double(), ball_height,
                          b + 0.9 * random_double());

      if ((center - point3(4, 0.2, 0)).length() > 0.9) {
        if (choose_mat < 0.1) {
          // diffuse
          const auto earth_texture = make_shared<image_texture>("earthmap.jpg");
          const shared_ptr<material> sphere_material =
              make_shared<diffuse_light>(earth_texture);
          world.add(make_shared<sphere>(center, 0.2, sphere_material));
        } else if (choose_mat < 0.4) {
          // diffuse
          const colour albedo = colour::random() * colour::random();
          const shared_ptr<material> sphere_material =
              make_shared<diffuse_light>(albedo);
          const point3 center2 = center + vec3(0, random_double(0, .5), 0);
          world.add(make_shared<animated_sphere>(center, center2, 0.0, 1.0, 0.2,
                                                 sphere_material));
        } else if (choose_mat < 0.8) {
          // diffuse
          const colour albedo = colour::random() * colour::random();
          const shared_ptr<material> sphere_material =
              make_shared<lambertian>(albedo);
          const point3 center2 = center + vec3(0, random_double(0, .5), 0);
          world.add(make_shared<animated_sphere>(center, center2, 0.0, 1.0, 0.2,
                                                 sphere_material));
        } else if (choose_mat < 0.95) {
          // metal
          const colour albedo = colour::random(0.5, 1);
          const double fuzz = random_double(0, 0.5);
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

  const auto material2 = make_shared<diffuse_light>(colour(0.4, 0.2, 0.1));
  world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

  const auto material3 = make_shared<metal>(colour(0.7, 0.6, 0.5), 0.0);
  world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

  return hittable_list(bvh_node::from_list(world, 0.0, 1.0));
}

auto earth() {
  auto earth_texture = make_shared<image_texture>("earthmap.jpg");
  auto earth_surface = make_shared<lambertian>(earth_texture);
  auto globe = make_shared<sphere>(point3(0, 0, 0), 2, earth_surface);

  return hittable_list(globe);
}

colour ray_colour(const ray &r, const colour &background, const hittable &world,
                  const int depth) {
  if (depth <= 0)
    return colour(0.0);

  hit_record rec;
  if (!world.hit(r, eps, infinity, rec))
    return background;

  ray scattered;
  colour attenuation;
  colour emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

  if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
    return emitted;

  return emitted +
         attenuation * ray_colour(scattered, background, world, depth - 1);
}

void render(const hittable_list &world, const camera &cam,
            const colour &background, const std::string &output,
            const int image_width, const int image_height) {
  const int samples_per_pixel = 100;
  const int max_depth = 500;
  const int tile_size = 32;

  image image(image_width, image_height);
  auto compute_tile = [&](const int tile_row, const int tile_col,
                          const int tile_height, const int tile_width) {
    for (int j = tile_row; j < tile_row + tile_height; ++j) {
      for (int i = tile_col; i < tile_col + tile_width; ++i) {
        colour pixel_colour;
        for (int s = 1; s <= samples_per_pixel; ++s) {
          const double u = (i + random_double()) / (image_width - 1);
          const double v = (j + random_double()) / (image_height - 1);
          const ray r = cam.get_ray(u, v);
          pixel_colour += ray_colour(r, background, world, max_depth);
        }
        image.set(j, i, pixel_colour / samples_per_pixel);
      }
    }
  };

  struct task {
    int tile_row, tile_col, tile_height, tile_width;
  };

  std::mutex task_list_mutex;
  std::queue<task> task_list;

  for (int tile_row = 0; tile_row < image_height; tile_row += tile_size) {
    for (int tile_col = 0; tile_col < image_width; tile_col += tile_size) {
      task_list.push({tile_row, tile_col,
                      std::min(image_height - tile_row, tile_size),
                      std::min(image_width - tile_col, tile_size)});
    }
  }

  const int max_threads = 4;
  std::cerr << "Starting render with " << max_threads << " threads..."
            << std::endl;
  const auto start_ms = get_time_ms();
  const int num_tasks = task_list.size();

  std::vector<std::thread> threads;
  for (int i = 0; i < max_threads; ++i) {
    threads.emplace_back([&]() {
      while (true) {
        std::unique_lock<std::mutex> lck(task_list_mutex);
        if (task_list.empty())
          break;
        const task next_task = task_list.front();
        task_list.pop();
        lck.unlock();

        compute_tile(next_task.tile_row, next_task.tile_col,
                     next_task.tile_height, next_task.tile_width);

        static long long last_update_ms = 0;
        const long long current_time_ms = get_time_ms();
        if (current_time_ms - last_update_ms > 1000) {
          const double elapsed_ms = current_time_ms - start_ms;
          const double remaining_tasks = task_list.size();
          const double done_tasks = num_tasks - remaining_tasks;
          const double estimated_remaining_ms =
              elapsed_ms / done_tasks * remaining_tasks;
          last_update_ms = current_time_ms;
          std::cout << "\r" << elapsed_ms / 1000 << "s elapsed, "
                    << estimated_remaining_ms / 1000 << "s remaining, "
                    << task_list.size() << "/" << num_tasks
                    << " tiles remaining... " << std::flush;
          image.write_png("output/progress.png");
        }
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  const auto end_ms = get_time_ms();
  const double elapsed_seconds =
      static_cast<double>(end_ms - start_ms) / 1000.0;
  std::cout << std::endl
            << "Done! Took " << elapsed_seconds << " seconds" << std::endl;

  image.write_png("output/progress.png");
  image.write_png(output);
}

int main(int argc, char *argv[]) {
  const std::string output_file = (argc > 1) ? argv[1] : "output/output.png";

  // Image
  const double aspect_ratio = 16.0 / 9.0;
  const int image_width = 1000;
  const int image_height = static_cast<int>(image_width / aspect_ratio);

  {
    // World
    const auto world = random_scene();

    // Camera
    const point3 lookfrom(13, 2, 3);
    const point3 lookat(0, 0, 0);
    const vec3 up(0, 1, 0);
    const double dist_to_focus = 10.0;
    const double aperture = 0.1;

    camera cam(lookfrom, lookat, up, 20, aspect_ratio, aperture, dist_to_focus,
               0.0, 1.0);

    const colour background(0, 0, 0.05); // (0.70, 0.80, 1.00);

    render(world, cam, background, output_file, image_width, image_height);
  }
}
