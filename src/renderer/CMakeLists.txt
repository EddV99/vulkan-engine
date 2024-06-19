add_library(renderer renderer.cpp 
                    renderer-vulkan.cpp
                    input.cpp)

target_link_libraries(renderer mesh util Vulkan::Vulkan glfw)
