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

#include "playing.hpp"
#include <libres.hpp>

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
}

playing_impl::playing_impl
(
    context& ctx,
    const screen_transition trans,
    const libgame::data_types::stage stage
):
    ctx_(ctx),
    stage_(stage),
    pscreen_
    (
        ctx.view.make_screen<screen>
        (
            stage,
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
                    ctx_.process_event
                    (
                        events::stage_selection_screen_show_request
                        {
                            data_types::screen_transitions::zoom_out{}
                        }
                    );
                },
                .handle_input_layout_change = [this](const libview::data_types::input_layout input_layout)
                {
                    libutil::log::info("[fsm <- screen] Input layout change: ", input_layout);
                    show_preview();
                }
            }
        )
    )
{
    //Load game state from database
    const auto& opt_game_state = ctx_.database.get_game_state();

    //Find stage state
    const auto opt_stage_state = find_stage_state(opt_game_state, stage);

    //Create game
    if(opt_stage_state)
    {
        const auto& stage_state = *opt_stage_state;
        const auto& board = stage_state.brd;

        pgame_ = std::make_unique<libgame::game>(stage, stage_state);

        //Initialize view
        pscreen_->set_score(get_score(board));
        pscreen_->set_time_s(stage_state.time_s);
        pscreen_->set_hi_score(stage_state.hi_score);
        pscreen_->set_move_count(stage_state.move_count);
        pscreen_->create_next_input(stage_state.input_tiles);
        pscreen_->insert_next_input();
        pscreen_->create_next_input(stage_state.next_input_tiles);
        pscreen_->set_board_tiles(board.tiles);
        show_preview();
        pscreen_->set_game_over_overlay_visible(is_overflowed(board));
    }
    else
    {
        pgame_ = std::make_unique<libgame::game>(stage);
        modify_game(&libgame::game::start);
    }

    ctx_.view.show_screen(pscreen_, trans);
}

} //namespace
