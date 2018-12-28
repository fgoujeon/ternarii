/*
Copyright 2018 Florian Goujeon

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

#include "board_input.hpp"

namespace libgame_2p
{

namespace
{
    const auto default_x_offset = 2;
    const auto default_rotation = 0;
}

board_input::board_input(const int player_index):
    player_index_(player_index),
    x_offset_(default_x_offset),
    rotation_(default_rotation)
{
}

event
board_input::set_tiles(const data_types::tile_pair& tiles)
{
    tiles_ = tiles;
    x_offset_ = default_x_offset;
    rotation_ = default_rotation;
    apply();

    return
    {
        events::next_input_insertion
        {
            player_index_,
            x_offset_,
            rotation_
        }
    };
}

std::vector<event>
board_input::shift_left()
{
    if(x_offset_ > 0)
    {
        --x_offset_;
        return apply();
    }

    return std::vector<event>();
}

std::vector<event>
board_input::shift_right()
{
    if(x_offset_ < column_count - 1)
    {
        ++x_offset_;
        return apply();
    }

    return std::vector<event>();
}

std::vector<event>
board_input::rotate()
{
    rotation_ = (rotation_ + 1) % 4;
    return apply();
}

std::vector<event>
board_input::apply()
{
    //adjust the offset so that the tiles stay inside the board
    if(x_offset_ >= column_count - 1 && (rotation_ == 0 || rotation_ == 2))
        x_offset_ = column_count - 2;

    return
    {
        events::input_layout_change
        {
            player_index_,
            x_offset_,
            rotation_
        }
    };
}

} //namespace libgame_2p
