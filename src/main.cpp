
#include "all_scenes.hpp"
#include "render.hpp"

/*

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

*/

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <tuple>
#include <vector>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"

static const char *vertexSource = R"(
  #version 150 core
  in vec2 position; 
  in vec2 texcoord; 
  out vec2 Texcoord;
  void main() { 
    Texcoord = texcoord; 
    gl_Position = vec4(position, 0.0, 1.0); 
  })";

static const char *fragmentSource = R"(
  #version 150 core
  in vec2 Texcoord;
  out vec4 outColor;
  uniform sampler2D tex;
  void main() {
    outColor = texture(tex, Texcoord); 
  })";

int main() {
  int width = 1200, height = 800;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Could not initialize SDL: " << SDL_GetError() << std::endl;
    return -1;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  SDL_Window *window = SDL_CreateWindow("Render Test", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, width, height,
                                        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

  if (window == nullptr) {
    std::cerr << "Could not create SDL window: " << SDL_GetError() << std::endl;
    SDL_Quit();
    return -1;
  }

  const SDL_GLContext gl_context = SDL_GL_CreateContext(window);
  if (gl_context == nullptr) {
    std::cerr << "Could not create OpenGL context: " << SDL_GetError()
              << std::endl;
    SDL_Quit();
    return -1;
  }

  if (glewInit() != GLEW_OK) {
    std::cerr << "Could not initialize GLEW" << std::endl;
    SDL_Quit();
    return -1;
  }

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint vbo;
  glGenBuffers(1, &vbo);

  //                           pos            uv
  const GLfloat vertices[] = {-1.0f, 1.0f,  0.0f, 0.0f,  //
                              1.0f,  1.0f,  1.0f, 0.0f,  //
                              1.0f,  -1.0f, 1.0f, 1.0f,  //
                              -1.0f, -1.0f, 0.0f, 1.0f}; //

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint ebo;
  glGenBuffers(1, &ebo);

  const GLuint elements[] = {0, 1, 2, 2, 3, 0};

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements,
               GL_STATIC_DRAW);

  const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexSource, 0);
  glCompileShader(vertexShader);

  const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentSource, 0);
  glCompileShader(fragmentShader);

  const GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glBindFragDataLocation(shaderProgram, 0, "outColor");
  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);

  const GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

  const GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
  glEnableVertexAttribArray(texAttrib);
  glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(GLfloat)));

  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  SDL_Event event;
  std::atomic<bool> running = true;

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();
  ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
  ImGui_ImplOpenGL3_Init("#version 150");

  const auto &scene = simple_scene();
  image screen(scene.cam.m_image_width, scene.cam.m_image_height);
  std::thread render_thread([&]() {
    render(running, scene.objects, scene.cam, screen, "build/simple_scene.png",
           10000, PER_FRAME);
  });

  while (running) {
    // Process SDL events
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      switch (event.type) {
      case SDL_QUIT:
        running = false;
        break;
      default:
        break;
      }
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair
    // to create a named window.
    {
      ImGui::Begin("Debug Info");
      ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                  1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
      ImGui::End();
    }
    screen.draw_to_screen_texture();

    // Clear the screen and draw the textured rectangle
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // Draw ImGui stuff
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap buffers
    SDL_GL_SwapWindow(window);
  }

  render_thread.join();

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();

  glDeleteTextures(1, &texture);
  glDeleteProgram(shaderProgram);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &ebo);

  glDeleteVertexArrays(1, &vao);

  SDL_Quit();
  return 0;
}
