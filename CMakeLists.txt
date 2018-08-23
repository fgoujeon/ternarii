cmake_minimum_required(VERSION 3.10)

project(App)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(app)
add_subdirectory(libgame)
add_subdirectory(libview)
add_subdirectory(libsdl)
