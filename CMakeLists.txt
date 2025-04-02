cmake_minimum_required(VERSION 3.20)

project(sdl_proj)
set(CMAKE_C_STANDARD 17)

set(ASSETS_PATH ${PROJECT_SOURCE_DIR}/assets)

set(HEADER_FILES
    ${CMAKE_SOURCE_DIR}/src/font.h
    ${CMAKE_SOURCE_DIR}/src/menu.h
    ${CMAKE_SOURCE_DIR}/src/game.h
    ${CMAKE_SOURCE_DIR}/src/card.h
    ${CMAKE_SOURCE_DIR}/src/connection.h
)

link_directories(${PROJECT_SOURCE_DIR}/lib)

# Comment this if you dont want additional information
# message(STATUS "Library directory: ${PROJECT_SOURCE_DIR}/lib")

file(GLOB lib_files "${PROJECT_SOURCE_DIR}/lib/*")

# Comment this if you dont want additional information
# message(STATUS "Found libraries: ${lib_files}")

file(GLOB_RECURSE sdl_proj_src
    ${CMAKE_SOURCE_DIR}/src/menu.c
    ${CMAKE_SOURCE_DIR}/src/font.c
    ${CMAKE_SOURCE_DIR}/src/card.c
    ${CMAKE_SOURCE_DIR}/src/game.c
    ${CMAKE_SOURCE_DIR}/src/connection.c
    ${CMAKE_SOURCE_DIR}/src/main.c
)

# Comment this if you dont want additional information
# message (STATUS "C files: ${sdl_proj_src}")

add_executable(sdl_proj ${HEADER_FILES} ${sdl_proj_src})

# set_target_properties(sdl_proj
#     PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY            "${PROJECT_SOURCE_DIR}/output"
#     RUNTIME_OUTPUT_DIRECTORY_DEBUG      "${PROJECT_SOURCE_DIR}/output"
#     RUNTIME_OUTPUT_DIRECTORY_RELEASE    "${PROJECT_SOURCE_DIR}/output"
# )


target_precompile_headers(sdl_proj PRIVATE pch/pch.h)

target_include_directories(sdl_proj PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(sdl_proj SDL3 SDL3_image SDL3_ttf)