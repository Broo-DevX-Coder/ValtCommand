# Base settings for the project
cmake_minimum_required(VERSION 3.20)
project(ValtCommand LANGUAGES CXX VERSION 1.0.0)

# C++ Standard settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories for binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Set RPATH 
if(APPLE)
    set(CMAKE_BUILD_RPATH "@loader_path;@loader_path/../lib")
    set(CMAKE_INSTALL_RPATH "@loader_path;@loader_path/../lib")
elseif(UNIX)
    set(CMAKE_BUILD_RPATH "$ORIGIN;$ORIGIN/../lib")
    set(CMAKE_INSTALL_RPATH "$ORIGIN;$ORIGIN/../lib")
endif()

# Enable compile commands for IDEs
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ASann and UBSan in Debug mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND NOT MSVC)
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer) 
    add_link_options(-fsanitize=address,undefined)
endif()

# Get Libraries
find_package(fmt 8.0 REQUIRED)
find_package(nlohmann_json 3.0 CONFIG REQUIRED)

# Add Directories for files
add_subdirectory(src)
add_subdirectory(app)

# CPack commands
set(CPACK_PACKAGE_NAME "ValtCommand")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_GENERATOR "TGZ;ZIP")
include(CPack)