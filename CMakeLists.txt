cmake_minimum_required(VERSION 3.10)

# set the project name
project(raytracer)
include_directories(raytracer PUBLIC src include)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-Ofast -ffast-math -Wall -Wextra -Wno-unused-parameter -pedantic")

# add the executable
add_executable(raytracer src/stb.cpp src/image.cpp src/main.cpp)


cmake_minimum_required(VERSION 3.16.3)

project(imgui-cmake LANGUAGES C CXX)


# ============ Raspbian detection ============

if(UNIX AND NOT APPLE)
        execute_process(COMMAND bash "-c" "cat /etc/os-release | grep '^ID=' | sed 's/ID=//'" OUTPUT_VARIABLE __OS_RELEASE)
        if(__OS_RELEASE MATCHES "raspbian")
                set(RASPBIAN ON)
        endif()
endif()

# ============================================


#Use openGLES instead of desktop GL. Automatically set if compiling on raspbian
#set(USE_GLES ON)

#Use the brcmGLESv2 driver instead of GLESv2. May be required on versions older than pi4
#set(USE_BROADCOM_DRIVER ON)

add_subdirectory("thirdparty")
