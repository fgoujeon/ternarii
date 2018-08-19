cmake_minimum_required(VERSION 3.10)

find_package(SDL2 REQUIRED)

file(GLOB_RECURSE INCLUDE_FILES include/*)
file(GLOB_RECURSE SRC_FILES src/*)

add_library(
	libview
    ${INCLUDE_FILES}
    ${SRC_FILES}
)

target_include_directories(
    libview
    PUBLIC include
)

target_link_libraries(
    libview
    PRIVATE
        SDL2::SDL2
)