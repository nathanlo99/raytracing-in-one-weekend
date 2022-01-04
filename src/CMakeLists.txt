cmake_minimum_required(VERSION 3.10)

# set the project name
project(raytracer)
include_directories(raytracer PUBLIC ../include)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Ofast -ffast-math -Wall -Wextra -Wno-unused-parameter -pedantic")

# add the executable
add_executable(raytracer image.cpp main.cpp)
