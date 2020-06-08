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

#include "playing.hpp"
#include "showing_stage_selection_screen.hpp"

namespace states
{

namespace
{
    std::optional<libgame::data_types::stage_state> find_stage_state
    (
        const std::optional<libdb::data_types::game_state>& opt_game_state,
        const libgame::data_types::stage stage
    )
    {
        if(!opt_game_state)
        {
            return std::nullopt;
        }

        const auto& game_state = *opt_game_state;

        const auto stage_state_it = game_state.stage_states.find(stage);
        if(stage_state_it == game_state.stage_states.end())
        {
            return std::nullopt;
        }

        return stage_state_it->second;
    }

    std::string_view get_pretty_name(const libgame::data_types::stage s)
    {
        switch(s)
        {
            case libgame::data_types::stage::purity_room:    return "PURITY ROOM";
            case libgame::data_types::stage::nullifier_room: return "NULLIFIER ROOM";
        }
        return "";
    }

    bool must_show_background(const libgame::data_types::stage s)
    {
        return s == libgame::data_types::stage::nullifier_room;
    }
}

playing::playing(fsm& f, const screen_transition trans, const libgame::data_types::stage stage):
    fsm_(f),
    stage_(stage),
    pscreen_
    (
        fsm_.view.make_screen<screen>
        (
            screen::callback_set
            {
                .handle_clear_request = [this]
                {
                    libutil::log::info("[fsm <- screen] Clear request");
                    modify_game(&libgame::game::start);
                },
                .handle_drop_request = [this](const libview::data_types::input_layout input_layout)
                {
                    libutil::log::info("[fsm <- screen] Drop request with layout: ", input_layout);
                    modify_game(&libgame::game::drop_input_tiles, input_layout);
                },
                .handle_exit_request = [this]
                {
                    libutil::log::info("[fsm <- screen] Exit request");
                    fsm_.set_state<states::showing_stage_selection_screen>(screen_transition::zoom_out);
                },
                .handle_input_layout_change = [this](const libview::data_types::input_layout input_layout)
                {
                    libutil::log::info("[fsm <- screen] Input layout change: ", input_layout);
                    mark_tiles_for_nullification();
                }
            },
            get_pretty_name(stage),
            must_show_background(stage)
        )
    )
{
    //Load game state from database
    const auto& opt_game_state = fsm_.database.get_game_state();

    //Find stage state
    const auto opt_stage_state = find_stage_state(opt_game_state, stage);

    //create game
    if(opt_stage_state)
    {
        pgame_ = std::make_unique<libgame::game>(stage, *opt_stage_state);

        const auto& board_tiles = pgame_->get_board_tiles();

        //initialize view
        pscreen_->set_score(get_score(board_tiles));
        pscreen_->set_hi_score(pgame_->get_hi_score());
        pscreen_->create_next_input(pgame_->get_input_tiles());
        pscreen_->insert_next_input();
        pscreen_->create_next_input(pgame_->get_next_input_tiles());
        pscreen_->set_board_tiles(board_tiles);
        mark_tiles_for_nullification();
        pscreen_->set_game_over_overlay_visible(is_overflowed(board_tiles));
    }
    else
    {
        pgame_ = std::make_unique<libgame::game>(stage);
        modify_game(&libgame::game::start);
    }

    fsm_.view.show_screen(pscreen_, trans);
}

} //namespace
