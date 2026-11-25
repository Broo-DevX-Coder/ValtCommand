# =======================================================================
# Modules and libraries
# =======================================================================
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# =======================================================================
# Create a dll or shred object
# =======================================================================
function(create_module NAME FOLDER)

    # Engine implementation and its internal/public declarations all remain
    # under the project source tree and include/ respectively.
    file(GLOB_RECURSE SOURCES
        ${FOLDER}/*.cpp
        ${PROJECT_SOURCE_DIR}/include/*.hpp
    )

    add_library(${NAME} SHARED ${SOURCES})

    target_include_directories(
        ${NAME}
        PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    target_compile_definitions(
        ${NAME}
        PRIVATE
        BUILDING_COMPILER_DLL
    )

    set_target_properties(${NAME} PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )


endfunction()

# =======================================================================
# Link objects with libs and thier self
# =======================================================================

create_module(valtcommand ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(valtcommand PRIVATE
    ${PROJECT_SOURCE_DIR}/libraries
)
target_compile_definitions(valtcommand PRIVATE FMT_HEADER_ONLY)

# =======================================================================
# Install: library, headers, and the find_package files
# =======================================================================

set_target_properties(valtcommand PROPERTIES
    VERSION ${LibVersion}
    SOVERSION ${LibSOVersion}
)

install(TARGETS valtcommand
    EXPORT ValtCommandTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/valtcommand
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT ValtCommandTargets
    FILE ValtCommandTargets.cmake
    NAMESPACE ValtCommand::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ValtCommand
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/ValtCommandConfigVersion.cmake
    VERSION ${LibVersion}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cmake/ValtCommandConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/ValtCommandConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ValtCommand
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/ValtCommandConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/ValtCommandConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ValtCommand
)
