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

#ifndef LIBGAME_PVP_GAME_HPP
#define LIBGAME_PVP_GAME_HPP

#include "events.hpp"
#include "data_types.hpp"
#include <memory>

namespace libgame
{

struct pvp_game
{
    public:
        using event = std::variant
        <
            events::end_of_game,
            events::pvp_board_tile_drop,
            events::pvp_input_tile_drop,
            events::pvp_next_input_creation,
            events::pvp_next_input_insertion,
            events::pvp_score_change,
            events::pvp_tile_merge,
            events::pvp_tile_nullification,
            events::start
        >;

        using event_list = std::vector<event>;

        pvp_game(int player_count, data_types::stage stage);

        ~pvp_game();

        void get_targeted_tiles
        (
            const int player_index,
            const data_types::input_layout& input_layout,
            libutil::matrix_coordinate_list& coords
        ) const;

        bool is_over() const;

        bool is_over(const int player_index) const;

        void start(event_list& events);

        void drop_input_tiles
        (
            const int player_index,
            const data_types::input_layout& layout,
            event_list& events
        );

    private:
        struct impl;
        std::unique_ptr<impl> pimpl_;
};

} //namespace

#endif
