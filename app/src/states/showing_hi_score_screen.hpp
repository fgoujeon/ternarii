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

#ifndef STATES_SHOWING_HI_SCORE_SCREEN_HPP
#define STATES_SHOWING_HI_SCORE_SCREEN_HPP

#include "../events.hpp"
#include "../context.hpp"
#include <libview/screens/hi_scores.hpp>

namespace states
{

struct showing_hi_score_screen
{
    void on_entry(const fgfsm::event_ref& event)
    {
        visit
        (
            event,

            [this](const events::hi_score_screen_show_request& event)
            {
                using screen = libview::screens::hi_scores;

                //get hi-scores
                auto scores = screen::score_map{};
                if(const auto& opt_game_state = ctx.database.get_game_state())
                {
                    const auto& game_state = opt_game_state.value();
                    const auto& stage_states = game_state.stage_states;
                    for(const auto& [stage, stage_state]: stage_states)
                    {
                        scores.insert({stage, stage_state.hi_score});
                    }
                }

                auto pscreen = ctx.view.make_screen<screen>
                (
                    scores,
                    screen::callback_set
                    {
                        .back_request = [this]
                        {
                            ctx.process_event
                            (
                                events::title_screen_show_request
                                {
                                    data_types::screen_transitions::left_to_right{}
                                }
                            );
                        }
                    }
                );
                ctx.view.show_screen(pscreen, event.transition);
            }
        );
    }

    context& ctx;
};

} //namespace

#endif
