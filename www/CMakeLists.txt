#Copyright 2018 - 2019 Florian Goujeon
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

add_custom_target(www)

file(GLOB_RECURSE SRC_FILES src/*)

add_custom_command(
    TARGET www POST_BUILD
    COMMAND ${CMAKE_COMMAND} ARGS -E make_directory ${EXECUTABLE_OUTPUT_PATH}
    COMMAND ${CMAKE_COMMAND} ARGS -E copy_if_different ${SRC_FILES} ${EXECUTABLE_OUTPUT_PATH}
)
