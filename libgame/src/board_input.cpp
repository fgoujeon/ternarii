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

namespace
{
    constexpr auto column_count = 6;

    constexpr auto default_layout = data_types::input_layout
    {
        .col_offset = 2,
        .rotation = 0
    };

    data_types::input_layout fix(const data_types::input_layout& in, const data_types::input_tile_array& tiles)
    {
        auto out = data_types::input_layout{};

        const auto tile_count = libcommon::data_types::get_tile_count(tiles);

        out.rotation = in.rotation % 4;

        const auto xmin = [&]
        {
            if(tile_count == 2 && out.rotation == 3)
            {
                return -1;
            }
            return 0;
        }();

        const auto xmax = [&]
        {
            if(tile_count == 1)
            {
                return column_count - 1;
            }
            if(tile_count == 2 && out.rotation == 1)
            {
                return column_count - 1;
            }
            return column_count - 2;
        }();

        out.col_offset = std::clamp(in.col_offset, xmin, xmax);

        return out;
    }
}

board_input::board_input(data_types::input_tile_array& tiles):
    tiles_(tiles),
    layout_(default_layout)
{
}

event board_input::set_tiles(const data_types::input_tile_array& tiles)
{
    tiles_ = tiles;
    layout_ = default_layout;

    return
    {
        events::next_input_insertion{layout_}
    };
}

void board_input::shift_left(event_list& events)
{
    --layout_.col_offset;
    layout_ = fix(layout_, tiles_);
    events.push_back(events::input_layout_change{layout_});
}

void board_input::shift_right(event_list& events)
{
    ++layout_.col_offset;
    layout_ = fix(layout_, tiles_);
    events.push_back(events::input_layout_change{layout_});
}

void board_input::rotate(event_list& events)
{
    const auto tile_count = libcommon::data_types::get_tile_count(tiles_);

    if(tile_count <= 1)
    {
        return;
    }

    ++layout_.rotation;

    //For tile pairs, temporarily shift left on rotation 3.
    if(tile_count == 2)
    {
        if(layout_.rotation == 3)
        {
            --layout_.col_offset;
        }
        if(layout_.rotation == 4)
        {
            ++layout_.col_offset;
        }
    }

    layout_ = fix(layout_, tiles_);
    events.push_back(events::input_layout_change{layout_});
}

} //namespace
