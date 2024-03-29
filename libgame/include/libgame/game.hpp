/*
Copyright 2018 - 2022 Florian Goujeon

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
#include "board_functions.hpp"
#include "data_types.hpp"
#include <memory>

namespace libgame
{

struct game
{
    public:
        game(data_types::stage stage);

        game(data_types::stage stage, const data_types::stage_state& state);

        ~game();

        const data_types::stage_state& get_state() const;

        bool is_over() const;

        void start(event_list& events);

        void drop_input_tiles
        (
            const data_types::input_layout& layout,
            event_list& events
        );

        void advance(double elapsed_s);

    private:
        struct impl;
        std::unique_ptr<impl> pimpl_;
};

} //namespace

#endif
