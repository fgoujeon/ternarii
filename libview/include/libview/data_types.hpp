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

#ifndef LIBVIEW_DATA_TYPES_HPP
#define LIBVIEW_DATA_TYPES_HPP

#include <libcommon/data_types.hpp>
#include <functional>
#include <optional>
#include <vector>
#include <array>
#include <ostream>

namespace libview::data_types
{

using board_tile_drop       = libcommon::data_types::board_tile_drop;
using board_tile_drop_list  = libcommon::data_types::board_tile_drop_list;
using board_tile_matrix     = libcommon::data_types::board_tile_matrix;
using column_nullifier_tile = libcommon::data_types::column_nullifier_tile;
using input_layout          = libcommon::data_types::input_layout;
using input_tile_drop       = libcommon::data_types::input_tile_drop;
using input_tile_drop_list  = libcommon::data_types::input_tile_drop_list;
using input_tile_matrix     = libcommon::data_types::input_tile_matrix;
using number_nullifier_tile = libcommon::data_types::number_nullifier_tile;
using number_tile           = libcommon::data_types::number_tile;
using row_nullifier_tile    = libcommon::data_types::row_nullifier_tile;
using tile                  = libcommon::data_types::tile;
using tile_merge            = libcommon::data_types::tile_merge;
using tile_merge_list       = libcommon::data_types::tile_merge_list;

enum class move
{
    clockwise_rotation,
    drop,
    left_shift,
    right_shift
};

} //namespace

#endif
