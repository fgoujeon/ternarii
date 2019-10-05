/*
Copyright 2018 - 2019 Florian Goujeon

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

#ifndef LIBVIEW_DATA_TYPES_HPP
#define LIBVIEW_DATA_TYPES_HPP

#include <functional>
#include <optional>
#include <vector>
#include <array>
#include <ostream>

namespace libview::data_types
{

enum class move
{
    clockwise_rotation,
    drop,
    left_shift,
    right_shift
};

struct tile
{
    int value = 0;
};

using opt_tile = std::optional<tile>;

template<size_t Size0, size_t Size1>
using tile_array = std::array<std::array<opt_tile, Size1>, Size0>;

using next_input_tile_array = std::array<opt_tile, 2>;
using input_tile_array = std::array<opt_tile, 2>;
using board_tile_array = tile_array<6, 10>;



struct tile_coordinate
{
    int x;
    int y;
};

using tile_coordinate_list = std::vector<tile_coordinate>;

struct tile_drop
{
    int column_index;
    int src_row_index;
    int dst_row_index;
};

using tile_drop_list = std::vector<tile_drop>;

struct tile_merge
{
    tile_coordinate_list src_tile_coordinates;
    tile_coordinate dst_tile_coordinate;
    int dst_tile_value;
};

using tile_merge_list = std::vector<tile_merge>;

} //namespace

#endif
