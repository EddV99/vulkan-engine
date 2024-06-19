cmake_minimum_required(VERSION 3.22)
project(vulkan-engine)

IF(WIN32)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) 

    add_subdirectory(vendor/glfw)
ELSE()
    set(CMAKE_CXX_FLAGS "-Wextra -Wall -fsanitize=address")
    find_package(glfw3 REQUIRED)
ENDIF()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

find_package(Vulkan REQUIRED)
#find_package(glfw3 REQUIRED)

project(vulkan-engine VERSION 1.0)

add_subdirectory(src)

add_executable(vulkan-engine src/entry.cpp)

target_include_directories(vulkan-engine PUBLIC 
    ${Vulkan_INCLUDE_DIR}
    "src/game"
    "src/math"
    "src/mesh"
    "src/networking"
    "src/physics"
    "src/renderer"
    "src/threads"
    "src/ui"
    "src/util"
    )

target_link_libraries(vulkan-engine PUBLIC game
                                           math
                                           mesh
                                           networking
                                           physics
                                           renderer
                                           threads
                                           ui
                                           util
                                           glfw
                                           ${GLFW_LIBRARIES}
                                           Vulkan::Vulkan)

