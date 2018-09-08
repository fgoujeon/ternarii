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

#ifndef GAME_HPP
#define GAME_HPP

#include "events.hpp"
#include "data_types.hpp"
#include <memory>

namespace libgame
{

struct game
{
    public:
        game();

        ~game();

        unsigned int get_score() const;

        const data_types::tile_pair& get_next_input_tiles() const;

        const data_types::tile_pair& get_input_tiles() const;

        const data_types::board_tile_grid& get_board_tiles() const;

        bool is_game_over() const;

        unsigned int get_input_x_offset() const;

        unsigned int get_input_rotation() const;

        event_list start();

        event_list shift_input_left();

        event_list shift_input_right();

        event_list rotate_input();

        event_list drop_input();

    private:
        struct impl;
        std::unique_ptr<impl> pimpl_;
};

} //namespace libgame

#endif
