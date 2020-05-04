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

#include "board_input.hpp"

namespace libgame
{

board_input::board_input
(
    data_types::input_tile_array& tiles,
    data_types::input_layout& layout
):
    tiles_(tiles),
    layout_(layout)
{
}

event board_input::set_tiles(const data_types::input_tile_array& tiles)
{
    tiles_ = tiles;
    layout_ = data_types::input_layout{};
    apply();

    return
    {
        events::next_input_insertion{layout_}
    };
}

void board_input::shift_left(event_list& events)
{
    if(layout_.x_offset > 0)
    {
        --layout_.x_offset;
        events.push_back(apply());
    }
}

void board_input::shift_right(event_list& events)
{
    if(layout_.x_offset < column_count - 1)
    {
        ++layout_.x_offset;
        events.push_back(apply());
    }
}

void board_input::rotate(event_list& events)
{
    layout_.rotation = (layout_.rotation + 1) % 4;
    events.push_back(apply());
}

events::input_layout_change board_input::apply()
{
    //adjust the offset so that the tiles stay inside the board
    if(layout_.x_offset >= column_count - 1 && (layout_.rotation == 0 || layout_.rotation == 2))
    {
        layout_.x_offset = column_count - 2;
    }

    return events::input_layout_change{layout_.x_offset, layout_.rotation};
}

} //namespace
