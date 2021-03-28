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

#include <libgame/pvp_game.hpp>
#include "input_generators.hpp"
#include "board.hpp"
#include <libutil/overload.hpp>
#include <algorithm>
#include <random>
#include <cmath>
#include <cassert>

namespace libgame
{

struct pvp_game::impl
{
    impl(const int player_count, const data_types::stage stage):
        player_count(player_count),
        input_gen(get_input_generator(stage)),
        states(player_count)
    {
        for(auto& state: states)
        {
            boards.push_back(board{state.board_tiles});
        }
    }

    events::pvp_next_input_creation generate_next_input(const int player_index)
    {
        const auto board_highest_tile_value =
            data_types::get_highest_tile_value(states[player_index].board_tiles)
        ;
        const auto board_tile_count = data_types::get_tile_count
        (
            states[player_index].board_tiles
        );

        //Generate a new input
        states[player_index].next_input_tiles = input_gen.generate
        (
            board_highest_tile_value,
            board_tile_count
        );

        return events::pvp_next_input_creation
        {
            player_index,
            states[player_index].next_input_tiles
        };
    }

    const int player_count;
    abstract_input_generator& input_gen;
    std::vector<data_types::stage_state> states;
    std::vector<board> boards;

    //used by drop_input_tiles()
    board::event_list board_events;
};

pvp_game::pvp_game(const int player_count, const data_types::stage stage):
    pimpl_(std::make_unique<impl>(player_count, stage))
{
}

pvp_game::~pvp_game() = default;

void pvp_game::get_targeted_tiles
(
    const int player_index,
    const data_types::input_layout& input_layout,
    libutil::matrix_coordinate_list& coords
) const
{
    return pimpl_->boards[player_index].get_targeted_tiles
    (
        pimpl_->states[player_index].input_tiles,
        input_layout,
        coords
    );
}

bool pvp_game::is_over() const
{
    return std::all_of
    (
        pimpl_->states.begin(),
        pimpl_->states.end(),
        [](const auto& state)
        {
            return data_types::is_overflowed(state.board_tiles);
        }
    );
}

bool pvp_game::is_over(const int player_index) const
{
    return data_types::is_overflowed(pimpl_->states[player_index].board_tiles);
}

void pvp_game::start(event_list& evts)
{
    evts.push_back(events::start{});

    for(int player_index = 0; auto& state: pimpl_->states)
    {
        //Clear data
        state.next_input_tiles = {};
        state.input_tiles = {};
        state.board_tiles = {};

        evts.push_back(events::pvp_score_change{player_index, 0});

        evts.push_back(pimpl_->generate_next_input(player_index));

        //Insert next input
        state.input_tiles = state.next_input_tiles;
        evts.push_back(events::pvp_next_input_insertion{player_index});

        evts.push_back(pimpl_->generate_next_input(player_index));

        ++player_index;
    }
}

void pvp_game::drop_input_tiles
(
    const int player_index,
    const data_types::input_layout& layout,
    event_list& evts
)
{
    if(is_over(player_index))
    {
        return;
    }

    //drop the input
    pimpl_->board_events.clear();
    pimpl_->boards[player_index].drop_input_tiles
    (
        pimpl_->states[player_index].input_tiles,
        layout,
        pimpl_->board_events
    );
    for(const auto board_event: pimpl_->board_events)
    {
        std::visit
        (
            libutil::overload
            {
                [&](const events::board_tile_drop& evt)
                {
                    evts.push_back
                    (
                        events::pvp_board_tile_drop
                        {
                            player_index,
                            evt.drops
                        }
                    );
                },
                [&](const events::input_tile_drop& evt)
                {
                    evts.push_back
                    (
                        events::pvp_input_tile_drop
                        {
                            player_index,
                            evt.drops
                        }
                    );
                },
                [&](const events::score_change& evt)
                {
                    evts.push_back
                    (
                        events::pvp_score_change
                        {
                            player_index,
                            evt.score
                        }
                    );
                },
                [&](const events::tile_merge& evt)
                {
                    evts.push_back
                    (
                        events::pvp_tile_merge
                        {
                            player_index,
                            evt.merges,
                            evt.granite_erosions
                        }
                    );
                },
                [&](const events::tile_nullification& evt)
                {
                    evts.push_back
                    (
                        events::pvp_tile_nullification
                        {
                            player_index,
                            evt.nullified_tile_coordinates
                        }
                    );
                }
            },
            board_event
        );
    }

    if(is_over())
    {
        evts.push_back(events::end_of_game{});
    }
    else
    {
        //move the next input into the input
        pimpl_->states[player_index].input_tiles =
            pimpl_->states[player_index].next_input_tiles
        ;
        evts.push_back(events::pvp_next_input_insertion{player_index});

        //create a new next input
        evts.push_back(pimpl_->generate_next_input(player_index));
    }
}

} //namespace
