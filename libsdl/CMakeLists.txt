cmake_minimum_required(VERSION 3.10)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

file(GLOB_RECURSE INCLUDE_FILES include/*)
file(GLOB_RECURSE SRC_FILES src/*)

add_library(
	libsdl
    ${INCLUDE_FILES}
    ${SRC_FILES}
)

target_include_directories(
    libsdl
    PUBLIC include
)

target_link_libraries(
    libsdl
    PUBLIC
        SDL2::SDL2
        SDL2_ttf
)
