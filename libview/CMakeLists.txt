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

set_directory_properties(PROPERTIES CORRADE_USE_PEDANTIC_FLAGS ON)

file(GLOB_RECURSE INCLUDE_FILES include/*)
file(GLOB_RECURSE SRC_FILES src/*)

add_library(libview ${INCLUDE_FILES} ${SRC_FILES})

set_property(
    TARGET libview
    PROPERTY CXX_STANDARD 20
)

target_include_directories(libview PUBLIC include)

target_link_libraries(
    libview
    PUBLIC
        libutil
    PRIVATE
        Magnum::Application
        Magnum::GL
        Magnum::Magnum
        Magnum::MagnumFont
        Magnum::MeshTools
        Magnum::Primitives
        Magnum::SceneGraph
        Magnum::Shaders
        Magnum::Text
        Magnum::Trade
)
