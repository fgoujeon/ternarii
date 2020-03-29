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

find_program(INKSCAPE_PROGRAM inkscape)
find_program(MAGNUM_DISTANCEFIELDCONVERTER_PROGRAM magnum-distancefieldconverter)
find_program(MAGNUM_FONTCONVERTER_PROGRAM magnum-fontconverter)

function(add_ttf_to_sdf_command FILENAME)
    set(TTF_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/fonts/${FILENAME}.ttf")
    set(SDF_FILE_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/out/fonts/${FILENAME}")
    set(SDF_TGA_FILE "${SDF_FILE_PREFIX}.tga")
    set(SDF_CONF_FILE "${SDF_FILE_PREFIX}.conf")

    add_custom_command(
        DEPENDS ${TTF_FILE}
        OUTPUT "${SDF_TGA_FILE}" "${SDF_CONF_FILE}"
        COMMAND ${CMAKE_COMMAND} ARGS -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/out/fonts"
        COMMAND ${MAGNUM_FONTCONVERTER_PROGRAM} ARGS --font FreeTypeFont --converter MagnumFontConverter --characters 'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ' --font-size 128 --output-size "512 512" --radius 32 "${TTF_FILE}" "${SDF_FILE_PREFIX}"
    )
endfunction()

#expects 100x100 px SVG
function(add_svg_to_sdf_command FILENAME)
    set(SVG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/images/${FILENAME}.svg")
    set(INTERMEDIATE_PNG_FILE ${FILENAME}.svg.png)
    set(SDF_TGA_FILE "${CMAKE_CURRENT_BINARY_DIR}/out/images/${FILENAME}.tga")

    add_custom_command(
        DEPENDS ${SVG_FILE}
        OUTPUT ${INTERMEDIATE_PNG_FILE}
        COMMAND ${INKSCAPE_PROGRAM} ARGS -z -e ${INTERMEDIATE_PNG_FILE} -a -4:-4:104:104 -w 1024 -h 1024 ${SVG_FILE}
    )

    add_custom_command(
        DEPENDS ${INTERMEDIATE_PNG_FILE}
        OUTPUT ${SDF_TGA_FILE}
        COMMAND ${CMAKE_COMMAND} ARGS -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/out/images"
        COMMAND ${MAGNUM_DISTANCEFIELDCONVERTER_PROGRAM} ARGS --output-size "64 64" --radius 32 --converter TgaImageConverter ${INTERMEDIATE_PNG_FILE} ${SDF_TGA_FILE}
    )
endfunction()

add_ttf_to_sdf_command(DejaVuSans)
add_svg_to_sdf_command(move_button)
add_svg_to_sdf_command(rotate_button)
add_svg_to_sdf_command(board_corner)

add_custom_target(
    res ALL
    SOURCES
        "${CMAKE_CURRENT_BINARY_DIR}/out/fonts/DejaVuSans.tga"
        "${CMAKE_CURRENT_BINARY_DIR}/out/fonts/DejaVuSans.conf"
        "${CMAKE_CURRENT_BINARY_DIR}/out/images/move_button.tga"
        "${CMAKE_CURRENT_BINARY_DIR}/out/images/rotate_button.tga"
        "${CMAKE_CURRENT_BINARY_DIR}/out/images/board_corner.tga"
)
