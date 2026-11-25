# Installation and linking

ValtCommand requires C++20. The installed package exports `ValtCommand::valtcommand`. Its package config includes the exported target file and does not call `find_dependency` for `fmt` or `nlohmann_json`. Public installed headers do not expose those dependencies: formatting and JSON are used by implementation files, and `DevTools::InspecturesReturn::JSON` is serialized `std::string` text. A host consuming the installed package needs only `find_package(ValtCommand CONFIG REQUIRED)` and the exported target. Include `<valtcommand/valtcommand.hpp>`.

## Build and install

From the repository root:

```sh
cmake -S Code -B build -DCMAKE_INSTALL_PREFIX=/chosen/prefix
cmake --build build --config Release
cmake --install build --config Release
```

Configure the package consumer examples with:

```sh
cmake -S DOCs/V1.0.0/Embedding-Guide/examples -B build/examples \
  -DCMAKE_PREFIX_PATH=/chosen/prefix
cmake --build build/examples --config Release
ctest --test-dir build/examples --output-on-failure
```

## Linux, macOS, and Windows

For a C++20 compiler and CMake, use the platform's normal CMake generator and install prefix. Separate apt, Homebrew, or Conan packages for fmt and nlohmann-json are not part of the installed consumer package interface. The source tree contains vendored headers under `Code/libraries/fmt/include` and `Code/libraries/nlohmann`.

## Consumer CMake

```cmake
find_package(ValtCommand CONFIG REQUIRED)
add_executable(host main.cpp)
target_link_libraries(host PRIVATE ValtCommand::valtcommand)
```

Set `CMAKE_PREFIX_PATH` to the installation prefix (or `ValtCommand_DIR` to its `lib/cmake/ValtCommand` directory). On Linux and macOS, the project sets install RPATH to `$ORIGIN;$ORIGIN/../lib` or `@loader_path;@loader_path/../lib`. This supports an executable finding a colocated library or one in a sibling `lib` directory. If your install layout differs, configure the host's RPATH or use the platform loader's library search configuration. Windows uses normal DLL search rules; the DLL must be discoverable at runtime.

## Source-build caveat

The checked-in vendored headers are in `Code/libraries/fmt/include` and `Code/libraries/nlohmann`, while `Code/src/CMakeLists.txt` currently adds private include paths named `third_party` and `third_party/fmt/include`, relative to `Code/src`. These paths do not match the vendored locations in this checkout. A source build without system-installed headers therefore remains unverified from the current CMake configuration; this does not alter the installed target's public dependency interface.
