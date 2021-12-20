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

#include "private_board.hpp"
#include <libgame/data_types.hpp>
#include <libutil/overload.hpp>
#include <memory>
#include <cmath>
#include <cassert>

namespace libgame
{

namespace
{
}

private_board::private_board(board& brd):
    board_(brd)
{
}

libutil::matrix_coordinate_list private_board::get_targeted_tiles
(
    const data_types::input_tile_matrix& input_tiles,
    const data_types::input_layout& input_layout
) const
{
    const auto result = apply_gravity_on_input
    (
        board_,
        input_tiles,
        input_layout
    );
    auto result2 = apply_nullifiers(result.brd);
    return result2.nullified_tiles_coords;
}

} //namespace
