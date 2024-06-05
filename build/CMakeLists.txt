cmake_minimum_required(VERSION 3.22)

project(vulkan-engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

IF(win32)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) 

    add_subdirectory(../vendor/glfw-3.4)
ELSE()
    set(CMAKE_CXX_FLAGS "-Wextra -Wall")
    find_package(glfw3 REQUIRED)
ENDIF()

find_package(Vulkan REQUIRED)
#find_package(glm REQUIRED)

add_library(game ../src/game/camera.cpp ../src/game/scene.cpp ../src/game/object.cpp)
add_library(math ../src/math/vector.cpp ../src/math/matrix.cpp)
add_library(mesh ../src/mesh/mesh.cpp)

add_library(networking ../src/networking/networking.cpp ../src/util/util.cpp)
add_library(physics ../src/physics/physics.cpp ../src/util/util.cpp)
add_library(threads ../src/threads/threads.cpp ../src/util/util.cpp)
add_library(ui ../src/ui/ui.cpp ../src/util/util.cpp)

add_library(renderer ../src/renderer/renderer.cpp 
                    ../src/renderer/renderer-vulkan.cpp 
                    ../src/renderer/renderer-core.hpp)
add_library(util ../src/util/util.cpp ../src/util/defines.hpp ../src/util/tiny_obj_loader.h)

target_link_libraries(math util)
target_link_libraries(mesh math util Vulkan::Vulkan)
target_link_libraries(renderer mesh util Vulkan::Vulkan glfw)

add_executable(vulkan-engine ../src/entry.cpp)

target_link_libraries(vulkan-engine PUBLIC glfw)
target_link_libraries(vulkan-engine PUBLIC Vulkan::Vulkan)
target_link_libraries(vulkan-engine PUBLIC math 
                                           mesh 
                                           networking
                                           physics
                                           threads
                                           game
                                           ui
                                           util
                                           renderer)


#target_link_libraries(vulkan-engine glm::glm)
