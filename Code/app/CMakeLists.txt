# include libraries
include(GNUInstallDirs)

# Stack all cpp source code files from app folder
file(GLOB_RECURSE APP_SOURCES
    "*.cpp"
)

# Create the target
add_executable(
    valtc
    ${APP_SOURCES}
)

# Add include files to executable
target_include_directories(
    valtc
    PRIVATE
    ../include
    ../libraries
)

# Link the Main app with Endine
target_link_libraries(
    valtc
    PRIVATE
    valtcommand
)

# Install target in the system
install(TARGETS valtc
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)