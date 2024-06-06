add_library(game camera.cpp scene.cpp object.cpp)
target_link_libraries(game Vulkan::Vulkan)
