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

#include "playing_versus.hpp"
#include "showing_title_screen.hpp"
#include <libres.hpp>

namespace states
{

playing_versus::playing_versus
(
    fsm& f,
    const screen_transition trans,
    const libgame::data_types::stage stage
):
    fsm_(f),
    p1_game_(stage),
    p2_game_(stage),
    pscreen_
    (
        fsm_.get_context().view.make_screen<screen>
        (
            stage,
            screen::callback_set
            {
                .handle_clear_request = [this]
                {
                    libutil::log::info("[fsm <- screen] Clear request");
                    modify_p1_game(&libgame::game::start);
                    modify_p2_game(&libgame::game::start);
                },
                .handle_exit_request = [this]
                {
                    libutil::log::info("[fsm <- screen] Exit request");
                    fsm_.set_state<states::showing_title_screen>(screen_transition::right_to_left);
                },
                .handle_p1_drop_request = [this](const libview::data_types::input_layout input_layout)
                {
                    libutil::log::info("[fsm <- screen] (P1) Drop request with layout: ", input_layout);
                    modify_p1_game(&libgame::game::drop_input_tiles, input_layout);
                },
                .handle_p1_input_layout_change = [this](const libview::data_types::input_layout input_layout)
                {
                    libutil::log::info("[fsm <- screen] (P1) Input layout change: ", input_layout);
                    mark_p1_tiles_for_nullification();
                },
                .handle_p2_drop_request = [this](const libview::data_types::input_layout input_layout)
                {
                    libutil::log::info("[fsm <- screen] (P2) Drop request with layout: ", input_layout);
                    modify_p2_game(&libgame::game::drop_input_tiles, input_layout);
                },
                .handle_p2_input_layout_change = [this](const libview::data_types::input_layout input_layout)
                {
                    libutil::log::info("[fsm <- screen] (P2) Input layout change: ", input_layout);
                    mark_p2_tiles_for_nullification();
                }
            }
        )
    )
{
    modify_p1_game(&libgame::game::start);
    modify_p2_game(&libgame::game::start);

    fsm_.get_context().view.show_screen(pscreen_, trans);
}

} //namespace
