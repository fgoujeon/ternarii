#Copyright 2018 - 2020 Florian Goujeon
#
#This file is part of Ternarii.
#
#Ternarii is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
#Ternarii is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License
#along with Ternarii.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.14)
project(ternarii)

find_program(GIT_PROGRAM git)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/modules/" ${CMAKE_MODULE_PATH})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --preload-file ${CMAKE_BINARY_DIR}/res/out@/res")

execute_process(
    COMMAND ${GIT_PROGRAM} "describe" "--dirty"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE PROJECT_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "PROJECT_VERSION = ${PROJECT_VERSION}")

execute_process(
    COMMAND ${GIT_PROGRAM} "log" "-1" "--format=%as"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    OUTPUT_VARIABLE PROJECT_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
message(STATUS "PROJECT_DATE = ${PROJECT_DATE}")

#SDF (Signed distance field) image configuration.
#It's better if "PNG_FILE_SIZE + 2 * RADIUS" is a power of two, because
#the bordered PNG will be downscaled to a power of two as well.
set(TERNARII_SDF_IMAGE_RADIUS 32) #SDF radius
set(TERNARII_SDF_IMAGE_PNG_SIZE 960) #width and height of PNG image directly generated from SVG

find_package(
    Magnum 2019.10 REQUIRED
    GL
    MagnumFont
    MeshTools
    Primitives
    SceneGraph
    Sdl2Application
    Shaders
    Text
    TgaImporter
    Trade
)
find_package(nlohmann_json 3.2.0 REQUIRED)

add_subdirectory(res)

add_subdirectory(app)
add_subdirectory(libcommon)
add_subdirectory(libdb)
add_subdirectory(libgame)
add_subdirectory(libutil)
add_subdirectory(libview)
add_subdirectory(www)
