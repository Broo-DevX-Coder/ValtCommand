function(create_module NAME FOLDER)

    file(GLOB_RECURSE SOURCES
        ${FOLDER}/*.cpp
        ../include/${FOLDER}/*.hpp
    )

    add_library(${NAME} SHARED ${SOURCES})

    target_include_directories(
        ${NAME}
        PUBLIC
        ../include
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