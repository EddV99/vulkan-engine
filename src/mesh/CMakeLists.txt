add_library(mesh mesh.cpp)
target_link_libraries(mesh math util Vulkan::Vulkan)
