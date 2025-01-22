cmake_minimum_required(VERSION 3.18) # Ensure modern CMake for compatibility
project(asyncio_example LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Compile in Debug
set(CMAKE_BUILD_TYPE Debug)

# Include the pybind11 subdirectory
add_subdirectory(pybind11)

# Add the pybind11 module
pybind11_add_module(asyncio_example src/main.cpp)

# Add compiler flags for coroutines support
target_compile_options(asyncio_example PRIVATE -fcoroutines)

# Pass version information from setup.py to the code
target_compile_definitions(asyncio_example PRIVATE VERSION_INFO="${EXAMPLE_VERSION_INFO}")
