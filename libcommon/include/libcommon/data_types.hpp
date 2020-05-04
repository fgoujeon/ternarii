/*
Copyright 2018 - 2020 Florian Goujeon

This file is part of Ternarii.

Ternarii is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Ternarii is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Ternarii.  If not, see <https://www.gnu.org/licenses/>.
*/

#ifndef LIBCOMMON_DATA_TYPES_HPP
#define LIBCOMMON_DATA_TYPES_HPP

#include <libutil/array2d.hpp>
#include <optional>
#include <vector>
#include <array>
#include <ostream>

namespace libcommon::data_types
{

struct tile
{
    int value = 0;
};

using opt_tile = std::optional<tile>;

/*
Convention for accessing the tiles:
tiles[column_index][row_index]

Convention of rows and columns:
[..]
[R4]
[R3]
[R2]
[R1]
[R0]
    [C0][C1][C2][C3][..]
*/
using tile_pair = std::array<tile, 2>;
using next_input_tile_array = std::array<opt_tile, 2>;
using input_tile_array = std::array<opt_tile, 2>;
using board_tile_array = libutil::array2d<opt_tile, 6, 10>;



struct input_layout
{
    int x_offset = 2;

    /*
    Rotation is expressed as number of 90 degree clockwise rotations.

    rotation = 0:
      --
      01
    rotation = 1:
      0-
      1-
    rotation = 2:
      --
      10
    rotation = 3:
      1-
      0-
    */
    int rotation = 0;

    bool operator==(const input_layout& r) const = default;
};

std::ostream& operator<<(std::ostream& l, const input_layout& r);



struct tile_coordinate
{
    int x = 0;
    int y = 0;
};

using tile_coordinate_list = std::vector<tile_coordinate>;

std::ostream& operator<<(std::ostream& l, const tile_coordinate& r);



struct tile_drop
{
    int column_index = 0;
    int src_row_index = 0;
    int dst_row_index = 0;
};

using tile_drop_list = std::vector<tile_drop>;

std::ostream& operator<<(std::ostream& l, const tile_drop& r);



struct tile_merge
{
    tile_coordinate_list src_tile_coordinates;
    tile_coordinate dst_tile_coordinate;
    int dst_tile_value = 0;
};

using tile_merge_list = std::vector<tile_merge>;

std::ostream& operator<<(std::ostream& l, const tile_merge& r);

} //namespace

#endif
