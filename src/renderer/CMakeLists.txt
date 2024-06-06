add_library(renderer renderer.cpp 
                    renderer-vulkan.cpp 
                    renderer-core.hpp)

target_link_libraries(renderer mesh util Vulkan::Vulkan glfw)
