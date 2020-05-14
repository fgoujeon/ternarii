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

function(copy_file SRC_FILE DST_FILE)
    get_filename_component(SRC_FILENAME "${SRC_FILE}" NAME)
    get_filename_component(DST_DIR "${DST_FILE}" DIRECTORY)
    file(COPY "${SRC_FILE}" DESTINATION "${DST_DIR}")
    file(RENAME "${DST_DIR}/${SRC_FILENAME}" "${DST_FILE}")
endfunction()

file(MAKE_DIRECTORY ${OUTPUT_DIR})

configure_file("${INPUT_DIR}/src/index.html.in" "${OUTPUT_DIR}/index.html")
copy_file("${INPUT_DIR}/src/index.css" "${OUTPUT_DIR}/index-${PROJECT_VERSION}.css")
copy_file("${INPUT_DIR}/res/icon.png"  "${OUTPUT_DIR}/icon-${PROJECT_VERSION}.png")
copy_file("${MAGNUM_JS}"               "${OUTPUT_DIR}/EmscriptenApplication-${PROJECT_VERSION}.js")
