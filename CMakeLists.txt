cmake_minimum_required(VERSION 3.5.0)
project(raytracer VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
find_package(OpenSSL REQUIRED)

add_executable(raytracer main.cpp)

target_include_directories(raytracer PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           "${PROJECT_SOURCE_DIR}/src/raymath"
                           "${PROJECT_SOURCE_DIR}/src/rayimage"
                           "${PROJECT_SOURCE_DIR}/src/rayscene"
                           )

add_subdirectory(./src/raymath)
add_subdirectory(./src/rayimage)
add_subdirectory(./src/rayscene)
add_subdirectory(./src/lodepng)

target_link_libraries(raytracer 
                      PUBLIC 
                      rayscene
                      raymath
                      rayimage
                      lodepng
                      OpenSSL::Crypto
                      )

# Testing configuration
enable_testing()
include(CTest)

add_executable(rtx_test test.cpp)
target_include_directories(rtx_test PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           "${PROJECT_SOURCE_DIR}/src/raymath"
                           "${PROJECT_SOURCE_DIR}/src/rayimage"
                           "${PROJECT_SOURCE_DIR}/src/rayscene"
                           )

target_link_libraries(rtx_test 
                      PUBLIC 
                      rayscene
                      raymath
                      rayimage
                      lodepng
                      OpenSSL::Crypto
                      )

# Set working directory for all tests to project root
set(TEST_WORKING_DIR ${CMAKE_SOURCE_DIR})

# Add the scene-specific tests with proper working directory


add_test(NAME TEST_TWO_SPHERES
         COMMAND rtx_test two-spheres-on-plane.json references/two-spheres-on-plane.png)


add_test(NAME TEST_TWO_SPHERES2
         COMMAND rtx_test two-spheres-on-plane.json references/two-spheres-on-plane.png)


add_test(NAME TEST_TWO_SPHERES3
         COMMAND rtx_test two-spheres-on-plane.json references/two-spheres-on-plane.png)


add_test(NAME TEST_TWO_SPHERES4
         COMMAND rtx_test two-spheres-on-plane.json references/two-spheres-on-plane.png)


add_test(NAME TEST_TWO_SPHERES5
         COMMAND rtx_test two-spheres-on-plane.json references/two-spheres-on-plane.png)

