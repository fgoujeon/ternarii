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

#ifndef LIBGAME_DATA_TYPES_HPP
#define LIBGAME_DATA_TYPES_HPP

#include <optional>
#include <vector>
#include <array>
#include <memory>
#include <ostream>

namespace libgame::data_types
{

template<typename T, size_t Size0, size_t Size1>
using array2d = std::array<std::array<T, Size1>, Size0>;

struct tile
{
    int value = 0;
};

using tile_pair = std::array<tile, 2>;

/*
Convention for accessing the tiles:
grid[column_index][row_index]

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
using tile_grid = array2d<std::optional<tile>, ColumnCount, RowCount>;

using board_tile_grid = tile_grid<6, 10>;

struct tile_coordinate
{
    int x = 0;
    int y = 0;
};

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
    std::vector<tile_coordinate> src_tile_coordinates;
    tile_coordinate dst_tile_coordinate;
    int dst_tile_value = 0;
};

using tile_merge_list = std::vector<tile_merge>;

std::ostream& operator<<(std::ostream& l, const tile_merge& r);



struct input_state
{
    tile_pair tiles;

    int x_offset = 2;

    //rotation = 0:
    //  --
    //  01
    //rotation = 1:
    //  0-
    //  1-
    //rotation = 2:
    //  --
    //  10
    //rotation = 3:
    //  1-
    //  0-
    int rotation = 0;
};



struct game_state
{
    int hi_score = 0;
    tile_pair next_input_tiles;
    input_state input;
    board_tile_grid board_tiles;
};

} //namespace

#endif
