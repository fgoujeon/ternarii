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

#include <libgame/game.hpp>
#include "input_generators.hpp"
#include "board.hpp"
#include <algorithm>
#include <random>
#include <cmath>
#include <cassert>

namespace libgame
{

struct game::impl
{
    impl(const data_types::stage stage):
        pinput_gen(make_input_generator(stage, random_device()))
    {
    }

    impl(const data_types::stage stage, const data_types::stage_state& s):
        pinput_gen(make_input_generator(stage, random_device())),
        state(s)
    {
    }

    events::next_input_creation generate_next_input()
    {
        const auto board_highest_tile_value = data_types::get_highest_tile_value(state.board_tiles);
        const auto board_tile_count = data_types::get_tile_count(state.board_tiles);

        //Generate a new input
        state.next_input_tiles = pinput_gen->generate
        (
            board_highest_tile_value,
            board_tile_count
        );

        return events::next_input_creation
        {
            state.next_input_tiles
        };
    }

    std::random_device random_device;
    std::unique_ptr<abstract_input_generator> pinput_gen;
    data_types::stage_state state;
    board board_{state.board_tiles};

    //used by drop_input_tiles()
    board::event_list board_events;
};

game::game(const data_types::stage stage):
    pimpl_(std::make_unique<impl>(stage))
{
}

game::game(const data_types::stage stage, const data_types::stage_state& state):
    pimpl_(std::make_unique<impl>(stage, state))
{
}

game::~game() = default;

const data_types::stage_state& game::get_state() const
{
    return pimpl_->state;
}

void game::get_targeted_tiles
(
    const data_types::input_layout& input_layout,
    libutil::matrix_coordinate_list& coords
) const
{
    return pimpl_->board_.get_targeted_tiles(pimpl_->state.input_tiles, input_layout, coords);
}

bool game::is_over() const
{
    return data_types::is_overflowed(pimpl_->state.board_tiles);
}

void game::start(event_list& evts)
{
    //Clear data
    pimpl_->state.next_input_tiles = {};
    pimpl_->state.input_tiles = {};
    pimpl_->state.board_tiles = {};
    pimpl_->state.move_count = 0;
    pimpl_->state.time_s = 0;

    evts.push_back(events::start{});
    evts.push_back(events::score_change{0});
    evts.push_back(events::move_count_change{pimpl_->state.move_count});

    evts.push_back(pimpl_->generate_next_input());

    //insert next input
    pimpl_->state.input_tiles = pimpl_->state.next_input_tiles;
    evts.push_back(events::next_input_insertion{});

    evts.push_back(pimpl_->generate_next_input());
}

void game::drop_input_tiles
(
    const data_types::input_layout& layout,
    event_list& evts
)
{
    if(is_over())
    {
        return;
    }

    //drop the input
    pimpl_->board_events.clear();
    pimpl_->board_.drop_input_tiles
    (
        pimpl_->state.input_tiles,
        layout,
        pimpl_->board_events
    );
    for(const auto& board_event: pimpl_->board_events)
    {
        std::visit
        (
            [&](const auto& evt)
            {
                evts.push_back(evt);
            },
            board_event
        );
    }

    auto& move_count = pimpl_->state.move_count;
    ++move_count;
    evts.push_back(events::move_count_change{move_count});

    if(is_over())
    {
        evts.push_back(events::end_of_game{});

        //Save hi-score
        const auto score = data_types::get_score(pimpl_->state.board_tiles);
        auto& hi_score = pimpl_->state.hi_score;
        if(hi_score < score)
        {
            hi_score = score;
            evts.push_back(events::hi_score_change{hi_score});
        }
    }
    else
    {
        //move the next input into the input
        pimpl_->state.input_tiles = pimpl_->state.next_input_tiles;
        evts.push_back(events::next_input_insertion{});

        //create a new next input
        evts.push_back(pimpl_->generate_next_input());
    }
}

void game::advance(const double elapsed_s)
{
    if(is_over())
    {
        return;
    }

    pimpl_->state.time_s += elapsed_s;
}

} //namespace
