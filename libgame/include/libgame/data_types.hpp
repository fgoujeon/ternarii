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

#include <libcommon/data_types.hpp>
#include <optional>
#include <vector>
#include <array>
#include <memory>
#include <ostream>

namespace libgame::data_types
{

using board_tile_array     = libcommon::data_types::board_tile_array;
using input_layout         = libcommon::data_types::input_layout;
using tile                 = libcommon::data_types::tile;
using tile_coordinate      = libcommon::data_types::tile_coordinate;
using tile_coordinate_list = libcommon::data_types::tile_coordinate_list;
using tile_drop            = libcommon::data_types::tile_drop;
using tile_drop_list       = libcommon::data_types::tile_drop_list;
using tile_merge           = libcommon::data_types::tile_merge;
using tile_merge_list      = libcommon::data_types::tile_merge_list;
using tile_pair            = libcommon::data_types::tile_pair;



struct input_state
{
    tile_pair tiles;
    input_layout layout;
};



struct game_state
{
    int hi_score = 0;
    tile_pair next_input_tiles;
    input_state input;
    board_tile_array board_tiles;
};

} //namespace

#endif
