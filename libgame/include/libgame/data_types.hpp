/*
Copyright 2018 - 2022 Florian Goujeon

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

#ifndef LIBGAME_DATA_TYPES_HPP
#define LIBGAME_DATA_TYPES_HPP

#include "constants.hpp"
#include <libutil/matrix.hpp>
#include <array>
#include <chrono>
#include <cmath>
#include <memory>
#include <optional>
#include <ostream>
#include <variant>
#include <vector>

namespace libgame::data_types
{

/*
Tile types
*/

namespace tiles
{
    struct number
    {
        int value = 0;
    };

    //Nullifies all the tiles of a column
    struct column_nullifier{};

    //Nullifies all the tiles of a row
    struct row_nullifier{};

    //Nullifies all the number tiles that have the same value than the one
    //placed below
    struct number_nullifier{};

    //Nullifies all the tiles of the outer columns
    struct outer_columns_nullifier{};

    struct granite
    {
        int thickness = 0;
    };

    //Adds its value to all the number tiles that have the same value than the
    //one
    struct adder
    {
        int value = 0;
    };

    std::ostream& operator<<(std::ostream& l, const number& r);
    std::ostream& operator<<(std::ostream& l, const column_nullifier& r);
    std::ostream& operator<<(std::ostream& l, const row_nullifier& r);
    std::ostream& operator<<(std::ostream& l, const number_nullifier& r);
    std::ostream& operator<<(std::ostream& l, const outer_columns_nullifier& r);
    std::ostream& operator<<(std::ostream& l, const granite& r);
    std::ostream& operator<<(std::ostream& l, const adder& r);
}

//Never reorder
using tile = std::variant
<
    tiles::number,
    tiles::column_nullifier,
    tiles::row_nullifier,
    tiles::number_nullifier,
    tiles::granite,
    tiles::adder,
    tiles::outer_columns_nullifier
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

using input_tile_matrix = basic_opt_tile_matrix
<
    constants::input_column_count,
    constants::input_row_count
>;

using board_tile_matrix = basic_opt_tile_matrix
<
    constants::board_column_count,
    constants::board_row_count
>;



struct board
{
    board_tile_matrix tiles;
};

//See board_functions.hpp for related functions



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
hypothetical 6*2 matrix.

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

bool is_valid
(
    const input_layout& layout,
    const input_tile_matrix& input_tiles
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



struct tile_value_change
{
    libutil::matrix_coordinate coordinate;
    int new_value = 0;
    int value_diff = 0;
};

using tile_value_change_list = std::vector<tile_value_change>;

std::ostream& operator<<(std::ostream& l, const tile_value_change& r);



struct adder_tile_application
{
    libutil::matrix_coordinate nullified_tile_coordinate;
    tile_value_change_list changes;
};

using adder_tile_application_list = std::vector<adder_tile_application>;

std::ostream& operator<<(std::ostream& l, const adder_tile_application& r);



struct granite_erosion
{
    libutil::matrix_coordinate coordinate;
    int new_thickness = 0;
};

using granite_erosion_list = std::vector<granite_erosion>;

std::ostream& operator<<(std::ostream& l, const granite_erosion& r);



//Never reorder
enum class stage
{
    purity_chapel,
    nullifier_room,
    triplet_pines_mall,
    granite_cave,
    math_classroom,
    waterfalls
};



struct stage_state
{
    double time_s = 0;
    int hi_score = 0;
    int move_count = 0;
    input_tile_matrix next_input_tiles;
    input_tile_matrix input_tiles;
    board brd;
};

} //namespace

#endif
