cmake_minimum_required(VERSION 3.22)

project(vulkan-engine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-Wextra -Wall")

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)

add_library(math ../src/math/vector.cpp ../src/math/matrix.cpp ../src/util/defines.hpp ../src/util/util.cpp)
add_library(mesh ../src/mesh/mesh.cpp ../src/util/tiny_obj_loader.h ../src/util/util.cpp)
add_library(networking ../src/networking/networking.cpp ../src/util/util.cpp)
add_library(physics ../src/physics/physics.cpp ../src/util/util.cpp)
add_library(scene ../src/scene/camera.cpp ../src/util/util.cpp)
add_library(threads ../src/threads/threads.cpp ../src/util/util.cpp)
add_library(ui ../src/ui/ui.cpp ../src/util/util.cpp)
add_library(renderer ../src/renderer/renderer.cpp 
                    ../src/renderer/renderer-vulkan.cpp 
                    ../src/renderer/renderer-core.hpp 
                    ../src/util/util.cpp)

#add_library(util ../src/util/util.cpp ../src/util/defines.hpp ../src/util/tiny_obj_loader.h)

#target_link_libraries(math util)

target_link_libraries(mesh math)
target_link_libraries(renderer mesh)

add_executable(vulkan-engine ../src/entry.cpp ../src/util/util.cpp)

target_link_libraries(vulkan-engine PUBLIC glfw)
target_link_libraries(vulkan-engine PUBLIC Vulkan::Vulkan)
target_link_libraries(vulkan-engine PUBLIC math 
                                           mesh 
                                           networking
                                           physics
                                           scene 
                                           threads
                                           ui
                                           renderer)


#target_link_libraries(vulkan-engine glm::glm)

# Add a custom target to run the executable
add_custom_target(run
    COMMAND vulkan-engine
    DEPENDS vulkan-engine
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
    COMMENT "Running the executable..."
)
