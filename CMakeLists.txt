# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

set (CMAKE_CXX_STANDARD 17)

project ("simple-raytracer")

find_package(glm CONFIG REQUIRED)
#find_package(assimp CONFIG REQUIRED)


add_executable(raytracer src/main.cpp "src/tga.hpp" "src/color.hpp" "src/framebuffer.hpp" "src/model.hpp" "src/rangeIterator.hpp" "src/objLoader.hpp" "src/shading.hpp" "src/camera.hpp" "src/triangle.hpp" "old/oldMainLoop.cpp")
target_link_libraries(raytracer PRIVATE glm)