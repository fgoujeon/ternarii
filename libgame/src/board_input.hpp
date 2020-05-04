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

#ifndef BOARD_INPUT_HPP
#define BOARD_INPUT_HPP

#include <libgame/events.hpp>
#include <libgame/data_types.hpp>
#include <array>
#include <vector>
#include <memory>

namespace libgame
{

class board_input
{
    public:
        static const int column_count = 6;

    public:
        board_input
        (
            data_types::input_tile_array& tiles,
            data_types::input_layout& layout
        );

        const data_types::input_tile_array& get_tiles() const
        {
            return tiles_;
        }

        const data_types::input_layout& get_layout() const
        {
            return layout_;
        }

        event set_tiles(const data_types::input_tile_array& tiles);

        void shift_left(event_list& events);

        void shift_right(event_list& events);

        void rotate(event_list& events);

    private:
        events::input_layout_change apply();

    private:
        data_types::input_tile_array& tiles_;
        data_types::input_layout& layout_;
};

} //namespace

#endif
