/*
Copyright 2018 - 2021 Florian Goujeon

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

#include <libgame/data_types.hpp>
#include <functional>
#include <optional>
#include <vector>
#include <array>
#include <ostream>

namespace libview::data_types
{

namespace tiles = libgame::data_types::tiles;
using board_tile_drop        = libgame::data_types::board_tile_drop;
using board_tile_drop_list   = libgame::data_types::board_tile_drop_list;
using board_tile_matrix      = libgame::data_types::board_tile_matrix;
using granite_erosion        = libgame::data_types::granite_erosion;
using granite_erosion_list   = libgame::data_types::granite_erosion_list;
using input_layout           = libgame::data_types::input_layout;
using input_tile_drop        = libgame::data_types::input_tile_drop;
using input_tile_drop_list   = libgame::data_types::input_tile_drop_list;
using input_tile_matrix      = libgame::data_types::input_tile_matrix;
using stage                  = libgame::data_types::stage;
using tile                   = libgame::data_types::tile;
using tile_merge             = libgame::data_types::tile_merge;
using tile_merge_list        = libgame::data_types::tile_merge_list;
using tile_value_change_list = libgame::data_types::tile_value_change_list;

enum class move_button
{
    left_shift,
    right_shift,
    clockwise_rotation,
    drop
};

} //namespace

#endif
