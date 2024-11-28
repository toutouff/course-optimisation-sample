cmake_minimum_required(VERSION 3.5.0)
project(raytracer VERSION 0.1.0 LANGUAGES C CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the thread preference to pthread
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Find OpenSSL package
find_package(OpenSSL REQUIRED)

# Add the executable target
add_executable(raytracer main.cpp)

# Enable fast math for supported compilers (GCC/Clang/MSVC), only in Release builds
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(raytracer PRIVATE -ffast-math)
        endif()
elseif(MSVC)
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(raytracer PRIVATE /fp:fast)
    endif()
endif()

option(ENABLE_MULTITHREADING "Enable multithreading support" ON)
message(STATUS "Multithreading support enabled:${ENABLE_MULTITHREADING}")
# Ajouter une définition de compilateur basée sur l'option
if(ENABLE_MULTITHREADING)
    # Ajouter des définitions de compilation pour le multithreading
    add_definitions(-DUSE_MULTITHREADING)
else()
    add_definitions(-DDISABLE_MULTITHREADING)
endif()

# Include directories for raytracer executable
target_include_directories(raytracer PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           "${PROJECT_SOURCE_DIR}/src/raymath"
                           "${PROJECT_SOURCE_DIR}/src/rayimage"
                           "${PROJECT_SOURCE_DIR}/src/rayscene"
                           )

# Add subdirectories for the modules
add_subdirectory(./src/raymath)
add_subdirectory(./src/rayimage)
add_subdirectory(./src/rayscene)
add_subdirectory(./src/lodepng)




# Link libraries for raytracer executable
target_link_libraries(raytracer 
                    PRIVATE Threads::Threads
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

# Add the rtx_test executable
add_executable(rtx_test test.cpp)



# Include directories for rtx_test executable
target_include_directories(rtx_test PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           "${PROJECT_SOURCE_DIR}/src/raymath"
                           "${PROJECT_SOURCE_DIR}/src/rayimage"
                           "${PROJECT_SOURCE_DIR}/src/rayscene"
                           )

# Link libraries for rtx_test executable
target_link_libraries(rtx_test 
                    PRIVATE Threads::Threads
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
add_test(NAME TEST_ISO_SPHERE
         COMMAND rtx_test iso-sphere-on-plane.json references/iso-sphere-on-plane.png)

# add_test(NAME TEST_TWO_SPHERES2
#          COMMAND rtx_test two-spheres-on-plane.json references/two-spheres-on-plane.png)

# add_test(NAME TEST_TWO_SPHERES3
#          COMMAND rtx_test two-spheres-on-plane.json references/two-spheres-on-plane.png)

# add_test(NAME TEST_TWO_SPHERES4
#          COMMAND rtx_test two-spheres-on-plane.json references/two-spheres-on-plane.png)

# add_test(NAME TEST_TWO_SPHERES5
#          COMMAND rtx_test two-spheres-on-plane.json references/two-spheres-on-plane.png)
