
#include "all_scenes.hpp"
#include "render.hpp"

int main() {
  if (false) {
    const auto scene = bright_scene();
    render_singlethreaded(scene.objects, scene.cam, "build/bright_scene.png",
                          scene.cam.m_image_width, scene.cam.m_image_height, 50,
                          PER_FRAME);
  }

  if (true) {
    const auto scene = instance_scene();
    render_debug(scene.objects, scene.cam, scene.cam.m_image_width,
                 scene.cam.m_image_height);
    render(scene.objects, scene.cam, "build/instance_scene.png",
           scene.cam.m_image_width, scene.cam.m_image_height, 10000, PER_FRAME);
  }
}
