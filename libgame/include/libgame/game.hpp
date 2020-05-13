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

        game(const data_types::game_state& state);

        ~game();

        const data_types::game_state& get_state() const;

        int get_score() const;

        int get_hi_score() const;

        const data_types::input_tile_array& get_next_input_tiles() const;

        const data_types::input_tile_array& get_input_tiles() const;

        const data_types::input_layout& get_input_layout() const;

        const data_types::board_tile_array& get_board_tiles() const;

        void get_targeted_tiles(data_types::tile_coordinate_list& coords) const;

        bool is_game_over() const;

        void start(event_list& events);

        void shift_input_left(event_list& events);

        void shift_input_right(event_list& events);

        void rotate_input(event_list& events);

        void drop_input_tiles(event_list& events);

    private:
        struct impl;
        std::unique_ptr<impl> pimpl_;
};

} //namespace

#endif
