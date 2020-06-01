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

#include "constants.hpp"
#include <libutil/matrix.hpp>
#include <variant>
#include <optional>
#include <vector>
#include <array>
#include <ostream>
#include <cmath>

namespace libcommon::data_types
{

/*
Tile types
*/

struct number_tile
{
    int value = 0;
};

//Nullifies all the tiles of a column
struct column_nullifier_tile{};

//Nullifies all the tiles of a row
struct row_nullifier_tile{};

//Nullifies all the number tiles that have the same value than the one
//placed below
struct number_nullifier_tile{};

std::ostream& operator<<(std::ostream& l, const number_tile& r);
std::ostream& operator<<(std::ostream& l, const column_nullifier_tile& r);
std::ostream& operator<<(std::ostream& l, const row_nullifier_tile& r);
std::ostream& operator<<(std::ostream& l, const number_nullifier_tile& r);

using tile = std::variant
<
    number_tile,
    column_nullifier_tile,
    row_nullifier_tile,
    number_nullifier_tile
>;



/*
Convention for accessing the tiles:
at(tiles, col, row)

Convention of rows and columns:
[..]
[R4]
[R3]
[R2]
[R1]
[R0]
    [C0][C1][C2][C3][..]
*/
template<int Cols, int Rows>
using basic_opt_tile_matrix = libutil::matrix<std::optional<tile>, Cols, Rows>;

//Get the row index of the lowest empty cell on the given column.
template<int Cols, int Rows>
std::optional<int> get_lowest_empty_cell
(
    const basic_opt_tile_matrix<Cols, Rows>& mat,
    const int col
)
{
    for(auto row = 0; row < Rows; ++row)
    {
        if(!at(mat, col, row))
        {
            return row;
        }
    }

    return std::nullopt;
}

template<int Cols, int Rows>
int get_tile_count
(
    const basic_opt_tile_matrix<Cols, Rows>& mat
)
{
    auto count = 0;
    for(const auto& opt_tile: mat)
    {
        if(opt_tile)
        {
            ++count;
        }
    }
    return count;
}



using input_tile_matrix = basic_opt_tile_matrix<constants::input_column_count, constants::input_row_count>;



/*
board_tile_matrix type and associated functions
*/

using board_tile_matrix = basic_opt_tile_matrix<constants::board_column_count, constants::board_row_count>;

bool is_overflowed(const board_tile_matrix& tiles);

int get_highest_tile_value(const board_tile_matrix& tiles);

int get_score(const board_tile_matrix& tiles);

int get_free_cell_count(const board_tile_matrix& tiles);



struct input_layout
{
    int col_offset = 2;

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

E.g. with col_offset = 3 and rotation = 1, input is laid out like so:
[---][---][---][0,0][0,1][---][---]
[---][---][---][1,0][1,1][---][---]

With such a layout:
- get_tile_coordinate(layout, {0, 0}) returns {3, 1}
- get_tile_coordinate(layout, {1, 0}) returns {3, 0}
*/
libutil::matrix_coordinate get_tile_coordinate
(
    const input_layout& layout,
    const libutil::matrix_coordinate& tile_coord //coordinate of tile in input
);



struct input_tile_drop
{
    libutil::matrix_coordinate input_coordinate;
    libutil::matrix_coordinate board_coordinate;
};

using input_tile_drop_list = std::vector<input_tile_drop>;

std::ostream& operator<<(std::ostream& l, const input_tile_drop& r);



struct board_tile_drop
{
    int col = 0;
    int src_row = 0;
    int dst_row = 0;
};

using board_tile_drop_list = std::vector<board_tile_drop>;

std::ostream& operator<<(std::ostream& l, const board_tile_drop& r);



struct tile_merge
{
    libutil::matrix_coordinate_list src_tile_coordinates;
    libutil::matrix_coordinate dst_tile_coordinate;
    int dst_tile_value = 0;
};

using tile_merge_list = std::vector<tile_merge>;

std::ostream& operator<<(std::ostream& l, const tile_merge& r);

} //namespace

#endif
