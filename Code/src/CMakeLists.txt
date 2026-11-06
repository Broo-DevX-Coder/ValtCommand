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
        ${PROJECT_SOURCE_DIR}/include
    )

    target_compile_definitions(
        ${NAME}
        PRIVATE
        BUILDING_COMPILER_DLL
    )

endfunction()

# =======================================================================
# Link objects with libs and thier self
# =======================================================================

create_module(engine .)

target_link_libraries(
    engine
    PUBLIC
    fmt
)

target_link_libraries(engine
    PRIVATE
        nlohmann_json::nlohmann_json
)
