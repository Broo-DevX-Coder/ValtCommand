cmake_minimum_required(VERSION 3.20)
project(ValtCommandRuntimeTests LANGUAGES CXX)
include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(VALTC_TESTS_USE_PACKAGE "Link an installed ValtCommand package" OFF)

if(VALTC_TESTS_USE_PACKAGE)
    find_package(ValtCommand CONFIG REQUIRED)
    set(VALTC_TEST_LIBRARY ValtCommand::valtcommand)
else()
    # Build the library from the sibling Code project without recursively adding tests.
    set(VALTC_TESTS OFF)
    add_subdirectory(.. ${CMAKE_CURRENT_BINARY_DIR}/valtcommand EXCLUDE_FROM_ALL)
    set(VALTC_TEST_LIBRARY valtcommand)
endif()

add_executable(valtcommand_runtime_tests
    main.cpp
    test_results.cpp
    test_functions.cpp
    test_errors.cpp
)
target_link_libraries(valtcommand_runtime_tests PRIVATE ${VALTC_TEST_LIBRARY})

# Bundle the runnable language examples and their status manifests for tests.
set(VALTC_DOCS_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/../../DOCs/V1.0.0/Language-references")
set(VALTC_DOCS_DEST "${CMAKE_CURRENT_BINARY_DIR}/docs/V1.0.0/Language-references")
file(COPY "${VALTC_DOCS_SOURCE}/examples" DESTINATION "${VALTC_DOCS_DEST}")
file(COPY "${VALTC_DOCS_SOURCE}/exceptions" DESTINATION "${VALTC_DOCS_DEST}")

if(BUILD_TESTING)
    add_test(NAME valtcommand_runtime_tests COMMAND valtcommand_runtime_tests)
    set_tests_properties(valtcommand_runtime_tests PROPERTIES
        WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
endif()
