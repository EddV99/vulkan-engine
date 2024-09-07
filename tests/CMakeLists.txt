# Google Test Framework
include(FetchContent)
FetchContent_Declare(
    googletest
    DOWNLOAD_EXTRACT_TIMESTAMP true
    URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

include(GoogleTest)

add_executable(hello_test test.cpp testDataReaderHelper.cpp)
gtest_discover_tests(hello_test)
target_link_libraries(hello_test game
                                 GTest::gtest_main
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
