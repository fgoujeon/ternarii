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
find_program(CONVERT_PROGRAM convert) #ImageMagick

function(add_ttf_to_sdf_command FILENAME)
    set(TTF_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/fonts/${FILENAME}.ttf")
    set(SDF_FILE_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/out/fonts/${FILENAME}")
    set(SDF_TGA_FILE "${SDF_FILE_PREFIX}.tga")
    set(SDF_CONF_FILE "${SDF_FILE_PREFIX}.conf")

    add_custom_command(
        DEPENDS ${TTF_FILE}
        OUTPUT "${SDF_TGA_FILE}" "${SDF_CONF_FILE}"
        COMMAND ${CMAKE_COMMAND} ARGS -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/out/fonts"
        COMMAND ${MAGNUM_FONTCONVERTER_PROGRAM} ARGS
            --font FreeTypeFont
            --converter MagnumFontConverter
            --characters "'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789γΩíš.-+(),\;:/_?! '"
            --font-size 128
            --output-size "512 512"
            --radius 32
            "${TTF_FILE}"
            "${SDF_FILE_PREFIX}"
    )
endfunction()

function(add_svg_to_sdf_command FILENAME OUTPUT_SIZE)
    set(SVG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/images/${FILENAME}.svg")
    set(PNG_FILE ${FILENAME}.png)
    set(BORDERED_PNG_FILE ${FILENAME}-bordered.png)
    set(SHIFTED_SDF_TGA_FILE "${FILENAME}.tga")
    set(SDF_TGA_FILE "${CMAKE_CURRENT_BINARY_DIR}/out/images/${FILENAME}.tga")

    #Convert SVG to PNG
    add_custom_command(
        DEPENDS ${SVG_FILE}
        OUTPUT ${PNG_FILE}
        COMMAND ${INKSCAPE_PROGRAM} ARGS
            -o ${PNG_FILE}
            --export-area-page
            -w ${TERNARII_SDF_IMAGE_PNG_SIZE}
            -h ${TERNARII_SDF_IMAGE_PNG_SIZE}
            ${SVG_FILE}
    )

    #Add borders to PNG (placeholders for distance field)
    add_custom_command(
        DEPENDS ${PNG_FILE}
        OUTPUT ${BORDERED_PNG_FILE}
        COMMAND ${CONVERT_PROGRAM} ARGS ${PNG_FILE} -bordercolor Black -border ${TERNARII_SDF_IMAGE_RADIUS} ${BORDERED_PNG_FILE}
    )

    #Generate distance field
    add_custom_command(
        DEPENDS ${BORDERED_PNG_FILE}
        OUTPUT ${SHIFTED_SDF_TGA_FILE}
        COMMAND ${CMAKE_COMMAND} ARGS -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/out/images"
        COMMAND ${MAGNUM_DISTANCEFIELDCONVERTER_PROGRAM} ARGS
            --output-size "${OUTPUT_SIZE} ${OUTPUT_SIZE}"
            --radius ${TERNARII_SDF_IMAGE_RADIUS}
            --converter TgaImageConverter
            ${BORDERED_PNG_FILE} ${SHIFTED_SDF_TGA_FILE}
    )

    #We need to add 1 row to the top and 1 column to the right because
    #the output of magnum-distancefieldconverter is slightly shifted for some
    #reason.
    add_custom_command(
        DEPENDS ${SHIFTED_SDF_TGA_FILE}
        OUTPUT ${SDF_TGA_FILE}
        COMMAND ${CONVERT_PROGRAM} ARGS ${SHIFTED_SDF_TGA_FILE} -gravity southeast -background black -splice 1x1 ${SDF_TGA_FILE}
    )
endfunction()

add_ttf_to_sdf_command(DejaVuSans)

add_svg_to_sdf_command(exit              64)
add_svg_to_sdf_command(board_corner      64)
add_svg_to_sdf_command(column_nullifier  64)
add_svg_to_sdf_command(logo              64)
add_svg_to_sdf_command(logo_text         256)
add_svg_to_sdf_command(move_button       64)
add_svg_to_sdf_command(number_nullifier  64)
add_svg_to_sdf_command(rotate_button     64)
add_svg_to_sdf_command(row_nullifier     64)
add_svg_to_sdf_command(tile_triplet      64)

#Generate libres.hpp
file(GLOB SRC_SVG_FILES src/images/*.svg)
unset(TGA_IMAGE_PATH_DEFINITIONS)
foreach(SRC_SVG_FILE ${SRC_SVG_FILES})
    get_filename_component(FILE_NAME "${SRC_SVG_FILE}" NAME_WE)
    set(DEF "constexpr auto ${FILE_NAME} = std::string_view{\"/res/images/${FILE_NAME}.tga\"};")
    set(TGA_IMAGE_PATH_DEFINITIONS "${TGA_IMAGE_PATH_DEFINITIONS}${DEF}\n")
endforeach()
configure_file(src/libres.hpp.in include/libres.hpp)

add_library(
    libres
    "src/dummy.cpp"
    "${CMAKE_CURRENT_BINARY_DIR}/out/fonts/DejaVuSans.conf"
    "${CMAKE_CURRENT_BINARY_DIR}/out/fonts/DejaVuSans.tga"
    "${CMAKE_CURRENT_BINARY_DIR}/out/images/exit.tga"
    "${CMAKE_CURRENT_BINARY_DIR}/out/images/board_corner.tga"
    "${CMAKE_CURRENT_BINARY_DIR}/out/images/column_nullifier.tga"
    "${CMAKE_CURRENT_BINARY_DIR}/out/images/logo.tga"
    "${CMAKE_CURRENT_BINARY_DIR}/out/images/logo_text.tga"
    "${CMAKE_CURRENT_BINARY_DIR}/out/images/move_button.tga"
    "${CMAKE_CURRENT_BINARY_DIR}/out/images/number_nullifier.tga"
    "${CMAKE_CURRENT_BINARY_DIR}/out/images/rotate_button.tga"
    "${CMAKE_CURRENT_BINARY_DIR}/out/images/row_nullifier.tga"
    "${CMAKE_CURRENT_BINARY_DIR}/out/images/tile_triplet.tga"
)

target_include_directories(
    libres
    PUBLIC
        ${CMAKE_CURRENT_BINARY_DIR}/include
)

target_link_options(
    libres
    PUBLIC
        --preload-file ${CMAKE_CURRENT_BINARY_DIR}/out@/res
)
