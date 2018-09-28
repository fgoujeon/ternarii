#Copyright 2018 Florian Goujeon
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

cmake_minimum_required(VERSION 3.10)

file(GLOB_RECURSE INCLUDE_FILES include/*)
file(GLOB_RECURSE SRC_FILES src/*)

add_library(
    libview
    ${INCLUDE_FILES}
    ${SRC_FILES}
)

set_property(
    TARGET libview
    PROPERTY CXX_STANDARD 17
)

target_include_directories(
    libview
    PUBLIC include
)

target_link_libraries(
    libview
    PRIVATE
        libsdl
)
