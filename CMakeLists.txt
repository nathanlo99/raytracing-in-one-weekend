cmake_minimum_required(VERSION 3.10)

# enable exporting commands for use with vscode
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# set the project name
project(raytracer)
include_directories(raytracer PUBLIC src thirdparty/stb)

find_package(OpenMP REQUIRED)

include_directories(thirdparty/imgui thirdparty/imgui/backends)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIR})

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Ofast -flto -ffast-math -Wall -Wextra -Wno-unused-parameter -pedantic -DUSE_FLOATS=0")
set(CMAKE_EXE_LINKER_FLAGS "-Ofast -flto -ffast-math")

# pull in boost libraries
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS random)
include_directories(${Boost_INCLUDE_DIRS})

# add the executable
file(GLOB raytracer_SRC CONFIGURE_DEPENDS "src/*.cpp" "src/objects/*.cpp")
include_directories(src/objects/ src/scenes/)
add_executable(raytracer ${raytracer_SRC} thirdparty/imgui/imgui.cpp
thirdparty/imgui/imgui_demo.cpp thirdparty/imgui/imgui_draw.cpp thirdparty/imgui/imgui_tables.cpp thirdparty/imgui/imgui_widgets.cpp
thirdparty/imgui/backends/imgui_impl_sdl.cpp thirdparty/imgui/backends/imgui_impl_opengl3.cpp)
target_link_libraries(raytracer ${Boost_LIBRARIES})
target_link_libraries(raytracer OpenMP::OpenMP_CXX)
target_link_libraries(raytracer ${SDL2_LIBRARIES})
target_link_libraries(raytracer ${OPENGL_LIBRARIES})
target_link_libraries(raytracer GLEW::GLEW)
