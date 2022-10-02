cmake_minimum_required(VERSION 3.10)

# set the project name
project(raytracer)
include_directories(raytracer PUBLIC src include)

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
file(GLOB raytracer_SRC CONFIGURE_DEPENDS "src/*.cpp")
add_executable(raytracer ${raytracer_SRC})
target_link_libraries(raytracer ${Boost_LIBRARIES})
