
#include "material_manager.hpp"
#include "util.hpp"

#include "colour.hpp"
#include "image.hpp"
#include "scenes/all_scenes.hpp"

#include <atomic>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

__attribute__((hot)) colour
ray_colour(const ray &r, const hittable &world, const int depth,
           const colour &contribution = colour(1.0)) {
  if (depth <= 0 || glm::length(contribution) < 1e-12) {
    // static std::atomic<int> early_exits = 0;
    // static int last_update = 0;
    // early_exits += depth;
    // if (early_exits >= last_update + 1000000) {
    //   last_update = early_exits;
    //   std::cout << early_exits << " early exits" << std::endl;
    // }
    return colour(0.0);
  }

  hit_record rec;
  if (!world.hit(r, eps, inf, rec))
    return colour(0.0);

  const colour emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);

  ray scattered;
  colour attenuation;
  if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
    return emitted;

  return emitted + attenuation * ray_colour(scattered, world, depth - 1,
                                            attenuation * contribution);
}

enum TileProtocol {
  PER_FRAME,
  PER_PIXEL,
  PER_LINE,
  PER_TILE,
};

inline colour normal_to_colour(const vec3 &normal) {
  assert(std::abs(glm::length(normal) - 1.0) < eps);
  return 0.5 * (normal + vec3(1.0));
}

void render_debug(const hittable_list &world, const camera &cam,
                  const int image_width, const int image_height) {
  image uv_image(image_width, image_height);
  image normals_image(image_width, image_height);
#pragma omp parallel for
  for (int j = 0; j < image_height; ++j) {
    for (int i = 0; i < image_width; ++i) {
      colour uv_colour(0.0);
      colour normal_colour(0.0);

      const real u = (i + 0.5) / image_width;
      const real v = (j + 0.5) / image_height;
      const ray r = cam.get_debug_ray(u, v);

      hit_record rec;
      if (!world.hit(r, eps, inf, rec))
        continue;
      normal_colour = normal_to_colour(rec.normal);
      uv_colour = vec3(1.0, rec.u, rec.v);

      uv_image.set(j, i, uv_colour);
      normals_image.set(j, i, normal_colour);
    }
    uv_image.write_png("build/output/debug_uvs.png");
    normals_image.write_png("build/output/debug_normals.png");
  }
}

void render_singlethreaded(const hittable_list &world, const camera &cam,
                           const std::string_view &output,
                           const int image_width, const int image_height,
                           const int samples_per_pixel, const TileProtocol) {
  const int max_depth = 100;

  image result_image(image_width, image_height);
  std::vector<long long> debug_times(image_width * image_height, 0);
  long long slowest_pixel = 0;

  const auto start_ms = util::get_time_ms();
  size_t pixels = 0;
  std::cout << "Starting render with 1 thread..." << std::endl;
  const auto pixel_jitters = util::get_sobol_sequence(2, samples_per_pixel);

#pragma omp parallel for
  for (int j = 0; j < image_height; ++j) {
    for (int i = 0; i < image_width; ++i) {
      const auto pixel_start_ns = util::get_time_ns();
      colour pixel_colour(0.0);
      for (int s = 0; s < samples_per_pixel; ++s) {
        const auto &[dx, dy] = pixel_jitters[s];
        const real u = (i + dx) / image_width;
        const real v = (j + dy) / image_height;
        const ray r = cam.get_ray(u, v);
        pixel_colour += ray_colour(r, world, max_depth);
      }
      pixels++;
      result_image.set(j, i,
                       pixel_colour / static_cast<real>(samples_per_pixel));

      static long long last_update_ms = 0;
      const long long current_time_ms = util::get_time_ms();

      const long long pixel_end_ns = util::get_time_ns();
      const long long pixel_ns = pixel_end_ns - pixel_start_ns;
      debug_times[j * image_width + i] = pixel_ns;
      slowest_pixel = std::max(slowest_pixel, pixel_ns);

      if (current_time_ms - last_update_ms > 1000) {
        last_update_ms = current_time_ms;
        const real elapsed_ms = current_time_ms - start_ms;
        const real done_tasks = pixels;
        const real num_tasks = image_width * image_height;
        const real remaining_tasks = num_tasks - done_tasks;
        const real tasks_per_ms = done_tasks / elapsed_ms;
        const real estimated_remaining_ms = remaining_tasks / tasks_per_ms;
        std::cout << "\r" << elapsed_ms / 1000 << "s elapsed, "
                  << tasks_per_ms * 1000 << " pixels per sec, "
                  << estimated_remaining_ms / 1000 << "s remaining, "
                  << remaining_tasks << "/" << num_tasks
                  << " pixels remaining... " << std::flush;
        result_image.write_png("build/output/progress.png");
      }
    }

    image debug_image(image_width, image_height);
#pragma omp parallel for
    for (int row = 0; row < image_height; ++row) {
      for (int i = 0; i < image_width; ++i) {
        const real pixel_ns = debug_times[row * image_width + i];
        const real ratio = pixel_ns / slowest_pixel;
        debug_image.set(row, i, colour(ratio));
      }
    }
    debug_image.write_png("build/output/debug.png");
  }

  const auto end_ms = util::get_time_ms();
  const real elapsed_seconds = (end_ms - start_ms) / 1000.0;
  std::cout << std::endl
            << "Done! Took " << elapsed_seconds << " seconds" << std::endl;

  result_image.write_png("build/output/progress.png");
  result_image.write_png(output);
}

void render(const hittable_list &world, const camera &cam,
            const std::string_view &output, const int image_width,
            const int image_height, const int samples_per_pixel,
            const TileProtocol protocol = PER_TILE) {
  const int max_threads = 4;
  const int max_depth = 100;

  const auto [tile_width, tile_height, tile_weight] = std::invoke(
      [&](const TileProtocol protocol) {
        switch (protocol) {
        case PER_FRAME:
          return std::make_tuple(image_width, image_height / max_threads, 8);
        case PER_PIXEL:
          return std::make_tuple(1, 1, samples_per_pixel);
        case PER_LINE:
          return std::make_tuple(image_width / max_threads, 1, 32);
        case PER_TILE:
          return std::make_tuple(16, 16, samples_per_pixel / 256);
        }
      },
      protocol);

  struct task {
    int tile_row, tile_col, tile_height, tile_width, sample_idx, tile_weight;
  };

  image result_image(image_width, image_height);
  std::vector<colour> framebuffer(image_width * image_height);
  std::vector<int> weights(image_width * image_height);
  std::mutex image_mutex;
  std::atomic<long long> num_samples = 0;
  const auto pixel_jitters = util::get_sobol_sequence(2, samples_per_pixel);

  auto compute_tile = [&](const task &tsk) {
    std::vector<colour> tmp_image(image_width * image_height);
    for (int j = tsk.tile_row; j < tsk.tile_row + tsk.tile_height; ++j) {
      for (int i = tsk.tile_col; i < tsk.tile_col + tsk.tile_width; ++i) {
        colour &pixel_colour = tmp_image[j * image_width + i];
        for (int s = 0; s < tsk.tile_weight; ++s) {
          const auto &[dx, dy] = pixel_jitters[tsk.sample_idx + s];
          const real u = (i + dx) / image_width;
          const real v = (j + dy) / image_height;
          const ray r = cam.get_ray(u, v);
          pixel_colour += ray_colour(r, world, max_depth);
        }
        num_samples += tsk.tile_weight;
      }
    }

    std::lock_guard<std::mutex> guard(image_mutex);
    for (int j = tsk.tile_row; j < tsk.tile_row + tsk.tile_height; ++j) {
      for (int i = tsk.tile_col; i < tsk.tile_col + tsk.tile_width; ++i) {
        const int idx = j * image_width + i;
        framebuffer[idx] += tmp_image[idx];
        weights[idx] += tsk.tile_weight;
        result_image.set(j, i,
                         framebuffer[idx] / static_cast<real>(weights[idx]));
      }
    }
  };

  std::vector<task> task_list;
  std::atomic<size_t> next_task_idx = 0;

  for (int samples = 0; samples < samples_per_pixel; samples += tile_weight) {
    for (int tile_row = 0; tile_row < image_height; tile_row += tile_height) {
      for (int tile_col = 0; tile_col < image_width; tile_col += tile_width) {
        const int task_height = std::min(image_height - tile_row, tile_height);
        const int task_width = std::min(image_width - tile_col, tile_width);
        const int task_samples =
            std::min(samples_per_pixel - samples, tile_weight);
        task_list.push_back({tile_row, tile_col, task_height, task_width,
                             samples, task_samples});
      }
    }
  }

  std::cerr << "Starting render with " << task_list.size() << " tasks and "
            << max_threads << " threads..." << std::endl;
  std::cerr << "There are " << material_manager::size() << " materials loaded"
            << std::endl;
  const auto start_ms = util::get_time_ms();
  const int num_tasks = task_list.size();

  std::vector<std::thread> threads;
  for (int i = 0; i < max_threads; ++i) {
    threads.emplace_back([&]() {
      while (true) {
        const int task_idx = next_task_idx++;
        if (task_idx >= num_tasks)
          break;
        compute_tile(task_list[task_idx]);

        static long long last_update_ms = util::get_time_ms();
        static int last_tasks = 0;
        static size_t last_samples = 0;
        const long long current_time_ms = util::get_time_ms();
        if (current_time_ms - last_update_ms > 1000) {
          const real update_ms = current_time_ms - last_update_ms;
          last_update_ms = current_time_ms;
          const real elapsed_ms = current_time_ms - start_ms;
          const int done_tasks = next_task_idx;
          const int remaining_tasks = std::max(0, num_tasks - done_tasks);
          const int this_updates_tasks = done_tasks - last_tasks;
          const int this_updates_samples = num_samples - last_samples;
          last_tasks = done_tasks;
          last_samples = num_samples;
          const real tasks_per_ms = this_updates_tasks / update_ms;
          const real samples_per_ms = this_updates_samples / update_ms;
          const real estimated_remaining_ms = remaining_tasks / tasks_per_ms;
          std::stringstream output_line;
          output_line << "\r" << elapsed_ms / 1000 << "s elapsed, "
                      << tasks_per_ms * 1000 << " tasks per sec, "
                      << samples_per_ms << " samples per ms, "
                      << estimated_remaining_ms / 1000 << "s remaining, "
                      << remaining_tasks << "/" << num_tasks
                      << " tasks remaining... ";
          const int target_line_length = 120;
          const int output_length = output_line.str().size();
          if (output_length < target_line_length)
            output_line << std::string(target_line_length - output_length, ' ');
          std::cout << output_line.str() << std::flush;
          result_image.write_png("build/output/progress.png");
        }
      }
    });
  }

  for (auto &thread : threads) {
    thread.join();
  }

  const auto end_ms = util::get_time_ms();
  const real elapsed_seconds = (end_ms - start_ms) / 1000.0;
  std::cout << std::endl
            << "Done! Took " << elapsed_seconds << " seconds" << std::endl;

  result_image.write_png("build/output/progress.png");
  result_image.write_png(output);
}

int main() {
  if (false) {
    const auto scene = bright_scene();
    render_singlethreaded(scene.objects, scene.cam, "build/bright_scene.png",
                          scene.cam.m_image_width, scene.cam.m_image_height, 50,
                          PER_FRAME);
  }

  if (true) {
    const auto scene = simple_scene();
    render_debug(scene.objects, scene.cam, scene.cam.m_image_width,
                 scene.cam.m_image_height);
    render(scene.objects, scene.cam, "build/simple_scene.png",
           scene.cam.m_image_width, scene.cam.m_image_height, 10000, PER_FRAME);
  }
}
