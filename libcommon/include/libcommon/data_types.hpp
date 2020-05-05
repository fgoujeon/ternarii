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

#include <libutil/matrix.hpp>
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

std::ostream& operator<<(std::ostream& l, const tile& r);



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
template<size_t ColumnCount, size_t RowCount>
using basic_opt_tile_matrix = libutil::matrix<opt_tile, ColumnCount, RowCount>;

using input_tile_array = basic_opt_tile_matrix<2, 2>;
using board_tile_array = basic_opt_tile_matrix<6, 10>;

//Get the row index of the lowest empty cell on the given column.
template<size_t ColumnCount, size_t RowCount>
std::optional<int> get_lowest_empty_cell
(
    const basic_opt_tile_matrix<ColumnCount, RowCount>& mat,
    const int column_index
)
{
    for(auto i = 0; i < RowCount; ++i)
    {
        if(!at(mat, column_index, i))
        {
            return i;
        }
    }

    return std::nullopt;
}



struct tile_coordinate
{
    int column_index = 0;
    int row_index = 0;
};

using tile_coordinate_list = std::vector<tile_coordinate>;

std::ostream& operator<<(std::ostream& l, const tile_coordinate& r);



struct input_layout
{
    int x_offset = 2;

    /*
    Rotation is expressed as number of 90 degree clockwise rotations.

    rotation = 0:
      [0,1][1,1]
      [0,0][1,0]
    rotation = 1:
      [0,0][0,1]
      [1,0][1,1]
    rotation = 2:
      [1,0][0,0]
      [1,1][0,1]
    rotation = 3:
      [1,0][1,0]
      [0,1][0,0]
    */
    int rotation = 0;

    bool operator==(const input_layout& r) const = default;
};

std::ostream& operator<<(std::ostream& l, const input_layout& r);

/*
Get the coordinate of the given tile, with given layout applied, in a
hypothetical 6*2 array.

E.g. with x_offset = 3 and rotation = 1, input is laid out like so:
[---][---][---][0,0][0,1][---][---]
[---][---][---][1,0][1,1][---][---]

With such a layout:
- get_tile_coordinate(layout, {0, 0}) returns {3, 1}
- get_tile_coordinate(layout, {1, 0}) returns {3, 0}
*/
tile_coordinate get_tile_coordinate
(
    const input_layout& layout,
    const tile_coordinate& tile_coord //coordinate of tile in input
);



struct input_tile_drop
{
    tile_coordinate input_coordinate;
    tile_coordinate board_coordinate;
};

using input_tile_drop_list = std::vector<input_tile_drop>;

std::ostream& operator<<(std::ostream& l, const input_tile_drop& r);



struct board_tile_drop
{
    int column_index = 0;
    int src_row_index = 0;
    int dst_row_index = 0;
};

using board_tile_drop_list = std::vector<board_tile_drop>;

std::ostream& operator<<(std::ostream& l, const board_tile_drop& r);



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
