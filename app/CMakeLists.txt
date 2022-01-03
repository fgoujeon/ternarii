#Copyright 2018 - 2022 Florian Goujeon
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

file(GLOB_RECURSE SRC_FILES src/*)

add_executable(app ${SRC_FILES})

set_target_properties(
    app PROPERTIES
    CXX_STANDARD 20
    OUTPUT_NAME "app-${PROJECT_VERSION}"
)

target_link_options(
    app
    PRIVATE
        "-sFORCE_FILESYSTEM=1"
        "-sDISABLE_EXCEPTION_CATCHING=0"
)

target_link_libraries(
    app
    PRIVATE
        fgfsm
        libdb
        libgame
        libview
)
