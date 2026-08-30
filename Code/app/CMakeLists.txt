# Stack all cpp source code files from app folder
file(GLOB_RECURSE APP_SOURCES
    "*.cpp"
)

# Create the target
add_executable(
    ValtCommand
    ${APP_SOURCES}
)

# Add include files to executable
target_include_directories(
    ValtCommand
    PUBLIC
    ../include
)

# Link the Main app with Endine
target_link_libraries(
    ValtCommand
    PRIVATE
    engine
)