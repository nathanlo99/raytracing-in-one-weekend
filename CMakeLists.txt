cmake_minimum_required(VERSION 3.10)

# set the project name
project(raytracer)
# add_subdirectory("thirdparty/imgui-cmake")
include_directories(raytracer PUBLIC src include)

# set(JSON_BuildTests OFF CACHE INTERNAL "")
# add_subdirectory("thirdparty/json")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Ofast -ffast-math -Wall -Wextra -Wno-unused-parameter -pedantic")
set(CMAKE_EXE_LINKER_FLAGS "-flto -Ofast")

# pull in boost libraries
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS random)
include_directories(${Boost_INCLUDE_DIRS})

# add the executable
add_executable(raytracer src/stb.cpp src/image.cpp src/main.cpp)
target_link_libraries(raytracer ${Boost_LIBRARIES})
# target_link_libraries(raytracer nlohmann_json::nlohmann_json)

# project(imgui-raytracer)
# include_directories(raytracer PUBLIC src include)
# add_executable(imgui-raytracer src/stb.cpp src/image.cpp src/imgui_main.cpp)
# target_link_libraries(imgui-raytracer IMGUI)
# target_link_libraries(imgui-raytracer nlohmann_json::nlohmann_json)
