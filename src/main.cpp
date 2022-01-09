
#include "util.hpp"

#include "animated_sphere.hpp"
#include "bvh_node.hpp"
#include "camera.hpp"
#include "colour.hpp"
#include "hittable_list.hpp"
#include "image.hpp"
#include "material.hpp"
#include "scenes/scenes.hpp"
#include "sphere.hpp"

#include <atomic>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

colour ray_colour(const ray &r, const colour &background, const hittable &world,
                  const int depth) {
  if (depth <= 0)
    return colour::random();

  hit_record rec;
  if (!world.hit(r, eps, infinity, rec))
    return background;

  const colour emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

  ray scattered;
  colour attenuation;
  if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
    return emitted;

  return emitted +
         attenuation * ray_colour(scattered, background, world, depth - 1);
}

void render_singlethreaded(const hittable_list &world, const camera &cam,
                           const colour &background, const std::string &output,
                           const int image_width, const int image_height) {
  const int samples_per_pixel = 10000;
  const int max_depth = 50;

  image result_image(image_width, image_height);

  const auto start_ms = get_time_ms();
  size_t pixels = 0;
  std::cout << "Starting render with 1 thread..." << std::endl;
  for (int j = 0; j < image_height; ++j) {
    for (int i = 0; i < image_width; ++i) {
      colour pixel_colour;
      for (int s = 1; s <= samples_per_pixel; ++s) {
        const double u = (i + random_double()) / (image_width - 1);
        const double v = (j + random_double()) / (image_height - 1);
        const ray r = cam.get_ray(u, v);
        pixel_colour += ray_colour(r, background, world, max_depth);
      }
      pixels++;
      result_image.set(j, i, pixel_colour / samples_per_pixel);

      static long long last_update_ms = 0;
      const long long current_time_ms = get_time_ms();
      if (current_time_ms - last_update_ms > 1000) {
        last_update_ms = current_time_ms;
        const double elapsed_ms = current_time_ms - start_ms;
        const double done_tasks = pixels;
        const double num_tasks = image_width * image_height;
        const double remaining_tasks = num_tasks - done_tasks;
        const double tasks_per_ms = done_tasks / elapsed_ms;
        const double estimated_remaining_ms = num_tasks / tasks_per_ms;
        std::cout << "\r" << elapsed_ms / 1000 << "s elapsed, "
                  << tasks_per_ms * 1000 << " pixels per sec, "
                  << estimated_remaining_ms / 1000 << "s remaining, "
                  << remaining_tasks << "/" << num_tasks
                  << " pixels remaining... " << std::flush;
        result_image.write_png("output/progress.png");
      }
    }
  }

  const auto end_ms = get_time_ms();
  const double elapsed_seconds = (end_ms - start_ms) / 1000.0;
  std::cout << std::endl
            << "Done! Took " << elapsed_seconds << " seconds" << std::endl;

  result_image.write_png("output/progress.png");
  result_image.write_png(output);
}

void render(const hittable_list &world, const camera &cam,
            const colour &background, const std::string &output,
            const int image_width, const int image_height) {
  const int samples_per_pixel = 10000;
  const int max_depth = 50;
  const int tile_size = std::max(image_width, image_height);
  const int tile_weight = 1;

  struct task {
    int tile_row, tile_col, tile_height, tile_width, tile_weight;
  };

  image result_image(image_width, image_height);
  std::vector<colour> framebuffer(image_width * image_height);
  std::vector<int> weights(image_width * image_height);
  std::mutex image_mutex;

  auto compute_tile = [&](const task &tsk) {
    std::vector<colour> tmp_image(image_width * image_height);
    for (int j = tsk.tile_row; j < tsk.tile_row + tsk.tile_height; ++j) {
      for (int i = tsk.tile_col; i < tsk.tile_col + tsk.tile_width; ++i) {
        colour &pixel_colour = tmp_image[j * image_width + i];
        for (int s = 1; s <= tsk.tile_weight; ++s) {
          const double u = (i + random_double()) / (image_width - 1);
          const double v = (j + random_double()) / (image_height - 1);
          const ray r = cam.get_ray(u, v);
          pixel_colour += ray_colour(r, background, world, max_depth);
        }
      }
    }

    std::lock_guard<std::mutex> guard(image_mutex);
    for (int j = tsk.tile_row; j < tsk.tile_row + tsk.tile_height; ++j) {
      for (int i = tsk.tile_col; i < tsk.tile_col + tsk.tile_width; ++i) {
        const int idx = j * image_width + i;
        framebuffer[idx] += tmp_image[idx];
        weights[idx] += tsk.tile_weight;
        result_image.set(j, i, framebuffer[idx] / weights[idx]);
      }
    }
  };

  std::vector<task> task_list;
  std::atomic<size_t> next_task_idx = 0;

  for (int samples = 0; samples < samples_per_pixel; samples += tile_weight) {
    for (int tile_row = 0; tile_row < image_height; tile_row += tile_size) {
      for (int tile_col = 0; tile_col < image_width; tile_col += tile_size) {
        task_list.push_back(
            {tile_row, tile_col, std::min(image_height - tile_row, tile_size),
             std::min(image_width - tile_col, tile_size),
             std::min(samples_per_pixel - samples, tile_weight)});
      }
    }
  }

  const int max_threads = 4;
  std::cerr << "Starting render with " << task_list.size() << " tasks and "
            << max_threads << " threads..." << std::endl;
  const auto start_ms = get_time_ms();
  const size_t num_tasks = task_list.size();

  std::vector<std::thread> threads;
  for (int i = 0; i < max_threads; ++i) {
    threads.emplace_back([&]() {
      while (true) {
        const size_t task_idx = next_task_idx++;
        if (task_idx >= num_tasks)
          break;
        compute_tile(task_list[task_idx]);

        static long long last_update_ms = 0;
        const long long current_time_ms = get_time_ms();
        if (current_time_ms - last_update_ms > 1000) {
          last_update_ms = current_time_ms;
          const double elapsed_ms = current_time_ms - start_ms;
          const double done_tasks = std::min<size_t>(num_tasks, next_task_idx);
          const double remaining_tasks = num_tasks - done_tasks;
          const double tasks_per_ms = done_tasks / elapsed_ms;
          const double estimated_remaining_ms =
              elapsed_ms / done_tasks * remaining_tasks;
          std::cout << "\r" << elapsed_ms / 1000 << "s elapsed, "
                    << tasks_per_ms * 1000 << " tasks per sec, "
                    << estimated_remaining_ms / 1000 << "s remaining, "
                    << remaining_tasks << "/" << num_tasks
                    << " tasks remaining... " << std::flush;
          result_image.write_png("output/progress.png");
        }
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  const auto end_ms = get_time_ms();
  const double elapsed_seconds = (end_ms - start_ms) / 1000.0;
  std::cout << std::endl
            << "Done! Took " << elapsed_seconds << " seconds" << std::endl;

  result_image.write_png("output/progress.png");
  result_image.write_png(output);
}

int main(int argc, char *argv[]) {
  if (true) {
    // Image
    const double aspect_ratio = 16.0 / 9.0;
    const int image_width = 1200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    // World
    auto world = bright_scene();
    const auto skybox_image =
        make_shared<image_texture>("../res/hdr_pack/5.hdr");
    const auto skybox_texture = make_shared<diffuse_light>(skybox_image);
    world.add(make_shared<sphere>(point3(0, 0, 0), 9000, skybox_texture));

    // Camera
    const point3 lookfrom(13, 2, 3);
    const point3 lookat(0, 0, 0);
    const vec3 up(0, 1, 0);
    const double dist_to_focus = 10.0;
    const double aperture = 0.1;

    const camera cam(lookfrom, lookat, up, 20, aspect_ratio, aperture,
                     dist_to_focus, 0.0, 1.0);

    const colour background(0.0, 0.0, 0.03);

    render_singlethreaded(world, cam, background, "bright_scene.png",
                          image_width, image_height);
  }

  {
    // Image
    const double aspect_ratio = 2.0;
    const int image_width = 1200;
    const int image_height = static_cast<int>(image_width / aspect_ratio);

    // World
    auto world = simple_scene();
    const auto skybox_image =
        make_shared<image_texture>("../res/hdr_pack/5.hdr");
    const auto skybox_texture = make_shared<diffuse_light>(skybox_image);
    world.add(make_shared<sphere>(point3(0, 0, 0), 9000, skybox_texture));

    // Camera
    const point3 lookfrom(0, 3, 6);
    const point3 lookat(0, 1, 0);
    const vec3 up(0, 1, 0);
    const double dist_to_focus = 10.0;
    const double aperture = 0.1;

    const camera cam(lookfrom, lookat, up, 50, aspect_ratio, aperture,
                     dist_to_focus, 0.0, 1.0);

    const colour background(0.70, 0.80, 1.00);

    render(world, cam, background, "simple_scene.png", image_width,
           image_height);
  }
}
