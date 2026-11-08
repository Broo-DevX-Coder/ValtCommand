# Base settings for the project
cmake_minimum_required(VERSION 3.20)
project(ValtCommand LANGUAGES CXX)

# C++ Standard settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Set RPATH 
set(CMAKE_BUILD_RPATH "$ORIGIN")
set(CMAKE_INSTALL_RPATH "$ORIGIN")

# Enable compile commands for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# ASann and UBSan in Debug mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT MSVC)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address,undefined)
endif()

# Get Libraries
find_package(fmt REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Add Directories for files
add_subdirectory(src)
add_subdirectory(app)

# Enable testing
enable_testing()
add_subdirectory(qa)
