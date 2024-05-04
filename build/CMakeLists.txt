cmake_minimum_required(VERSION 3.22)

project(vulkan-engine)

set(CMAKE_CXX_STANDARD 17)

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)

file(GLOB_RECURSE SRC "../src/*.cpp")

add_executable(vulkan-engine ${SRC})

target_link_libraries(vulkan-engine glfw)
target_link_libraries(vulkan-engine Vulkan::Vulkan)
target_link_libraries(vulkan-engine glm::glm)
