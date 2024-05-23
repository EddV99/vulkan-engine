cmake_minimum_required(VERSION 3.22)

project(vulkan-engine)

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)

add_library(math ../src/math/vector.cpp ../src/math/matrix.cpp)
add_library(mesh ../src/mesh/mesh.cpp)
add_library(networking ../src/networking/networking.cpp)
add_library(physics ../src/physics/physics.cpp)
add_library(scene ../src/scene/camera.cpp)
add_library(threads ../src/threads/threads.cpp)
add_library(ui ../src/ui/ui.cpp)
add_library(util ../src/util/util.cpp ../src/util/defines.hpp ../src/util/tiny_obj_loader.h)
add_library(renderer ../src/renderer/renderer.cpp ../src/renderer/renderer-vulkan.cpp ../src/renderer/renderer-core.hpp)

target_link_libraries(mesh math util)
target_link_libraries(renderer mesh util)

add_executable(vulkan-engine ../src/entry.cpp)

target_link_libraries(vulkan-engine PUBLIC glfw)
target_link_libraries(vulkan-engine PUBLIC Vulkan::Vulkan)
target_link_libraries(vulkan-engine PUBLIC math 
                                           mesh 
                                           networking
                                           physics
                                           scene 
                                           threads
                                           ui
                                           util
                                           renderer)


#target_link_libraries(vulkan-engine glm::glm)

