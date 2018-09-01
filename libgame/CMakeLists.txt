cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE INCLUDE_FILES include/*)
file(GLOB_RECURSE SRC_FILES src/*)

add_library(
	libgame
    ${INCLUDE_FILES}
    ${SRC_FILES}
)

target_include_directories(
    libgame
    PUBLIC include
)
