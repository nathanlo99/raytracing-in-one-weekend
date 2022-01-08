cmake_minimum_required(VERSION 3.10)

# set the project name
project(raytracer)
add_subdirectory("thirdparty/imgui-cmake")
include_directories(raytracer PUBLIC src include)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Ofast -ffast-math -Wall -Wextra -Wno-unused-parameter -pedantic")
set(CMAKE_EXE_LINKER_FLAGS "-flto -Ofast")

# add the executable
add_executable(raytracer src/stb.cpp src/image.cpp src/main.cpp)

project(imgui-raytracer)
include_directories(raytracer PUBLIC src include)
add_executable(imgui-raytracer src/stb.cpp src/image.cpp src/imgui_main.cpp)
target_link_libraries(imgui-raytracer IMGUI)
