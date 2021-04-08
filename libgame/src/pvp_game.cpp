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

namespace
{
    struct player_state
    {
        double granite_counter = 0;
        int move_count = 0;
        data_types::input_tile_matrix next_input_tiles;
        data_types::input_tile_matrix input_tiles;
        data_types::board_tile_matrix board_tiles;
    };
}

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

    void generate_next_input
    (
        const int player_index,
        event_list& evts
    )
    {
        auto& state = states[player_index];

        const auto board_highest_tile_value =
            data_types::get_highest_tile_value(state.board_tiles)
        ;
        const auto board_tile_count = data_types::get_tile_count
        (
            state.board_tiles
        );

        if(state.granite_counter <= 0)
        {
            //Generate a new input
            state.next_input_tiles = input_gen.generate
            (
                board_highest_tile_value,
                board_tile_count
            );

            evts.push_back
            (
                events::pvp_next_input_creation
                {
                    player_index,
                    state.next_input_tiles
                }
            );
        }
        else
        {
            //Generate next input
            auto next_input_tiles = input_gen.generate
            (
                board_highest_tile_value,
                board_tile_count
            );

            //Find a number tile in the generated input
            const auto pnumber_tile = [&]() -> std::optional<data_types::tile>*
            {
                for(auto& opt_tile: next_input_tiles)
                {
                    if(opt_tile.has_value())
                    {
                        if(const auto ptile = std::get_if<data_types::tiles::number>(&opt_tile.value()))
                        {
                            return &opt_tile;
                        }
                    }
                }
                return nullptr;
            }();

            //Substitute the number tile with a granite
            if(pnumber_tile)
            {
                const auto granite_thickness = std::min(3, static_cast<int>(state.granite_counter));

                *pnumber_tile = data_types::tiles::granite{granite_thickness};

                state.granite_counter -= granite_thickness;

                evts.push_back
                (
                    events::pvp_granite_counter_change
                    {
                        player_index,
                        static_cast<int>(state.granite_counter)
                    }
                );
            }

            state.next_input_tiles = next_input_tiles;

            evts.push_back
            (
                events::pvp_next_input_creation
                {
                    player_index,
                    next_input_tiles
                }
            );
        }
    }

    const int player_count;
    int move_count = 0;
    abstract_input_generator& input_gen;
    std::vector<player_state> states;
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
        state = player_state{};

        evts.push_back(events::pvp_score_change{player_index, 0});

        pimpl_->generate_next_input(player_index, evts);

        //Insert next input
        state.input_tiles = state.next_input_tiles;
        evts.push_back(events::pvp_next_input_insertion{player_index});

        pimpl_->generate_next_input(player_index, evts);

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

    auto& state = pimpl_->states[player_index];
    auto& board = pimpl_->boards[player_index];

    //drop the input
    pimpl_->board_events.clear();
    board.drop_input_tiles
    (
        state.input_tiles,
        layout,
        pimpl_->board_events
    );

    auto merge_count = 0;
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
                    ++merge_count;
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
        state.input_tiles = state.next_input_tiles;
        evts.push_back(events::pvp_next_input_insertion{player_index});

        //create a new next input
        pimpl_->generate_next_input(player_index, evts);
    }

    //Update counters
    const auto average_move_count = 3;
    const auto malus_strength = 8.0;
    ++pimpl_->move_count;
    ++state.move_count;
    if(pimpl_->move_count == average_move_count * pimpl_->player_count)
    {
        for(auto i = 0; i < pimpl_->player_count; ++i)
        {
            auto& state = pimpl_->states[i];

            const auto balance = average_move_count - state.move_count;
            const auto positive_balance = std::max(0, balance);
            state.granite_counter += malus_strength * positive_balance / pimpl_->player_count;

            evts.push_back
            (
                events::pvp_granite_counter_change
                {
                    i,
                    static_cast<int>(state.granite_counter)
                }
            );

            state.move_count = 0;
        }

        pimpl_->move_count = 0;
    }
}

} //namespace
