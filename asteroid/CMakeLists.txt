cmake_minimum_required(VERSION 3.28)
project(asteroid)

set(CMAKE_CXX_STANDARD 20)

# Set compiler-specific debug flags
if(MSVC)
    # MSVC flags
    set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od /RTC1")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
else()
    # GCC/Clang flags
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Create the executable
add_executable(asteroid main.cpp)

# Set output directories for different build types
set_target_properties(asteroid PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release
)
