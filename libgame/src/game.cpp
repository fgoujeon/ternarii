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
#include <algorithm>
#include <random>
#include <cmath>
#include <cassert>

namespace libgame
{

struct game::impl
{
    impl(const data_types::stage stage):
        input_gen(get_input_generator(stage))
    {
    }

    impl(const data_types::stage stage, const data_types::stage_state& s):
        input_gen(get_input_generator(stage)),
        state(s)
    {
    }

    events::next_input_creation generate_next_input()
    {
        const auto board_highest_tile_value = get_highest_tile_value(state.brd);
        const auto board_tile_count = get_tile_count(state.brd);

        //Generate a new input
        state.next_input_tiles = input_gen.generate
        (
            board_highest_tile_value,
            board_tile_count
        );

        return events::next_input_creation
        {
            state.next_input_tiles
        };
    }

    abstract_input_generator& input_gen;
    data_types::stage_state state;
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

bool game::is_over() const
{
    return is_overflowed(pimpl_->state.brd);
}

void game::start(event_list& events)
{
    //Clear data
    pimpl_->state.next_input_tiles = {};
    pimpl_->state.input_tiles = {};
    pimpl_->state.brd = {};
    pimpl_->state.move_count = 0;
    pimpl_->state.time_s = 0;

    events.push_back(events::start{});
    events.push_back(events::score_change{0});
    events.push_back(events::move_count_change{pimpl_->state.move_count});

    events.push_back(pimpl_->generate_next_input());

    //insert next input
    pimpl_->state.input_tiles = pimpl_->state.next_input_tiles;
    events.push_back(events::next_input_insertion{});

    events.push_back(pimpl_->generate_next_input());
}

void game::drop_input_tiles
(
    const data_types::input_layout& layout,
    event_list& events
)
{
    if(is_over())
    {
        return;
    }

    //drop the input
    {
        auto result = libgame::data_types::drop_input_tiles
        (
            pimpl_->state.brd,
            pimpl_->state.input_tiles,
            layout
        );

        pimpl_->state.brd = result.brd;

        for(const auto& event: result.events)
            events.push_back(event);
    }

    auto& move_count = pimpl_->state.move_count;
    ++move_count;
    events.push_back(events::move_count_change{move_count});

    if(is_over())
    {
        events.push_back(events::end_of_game{});

        //Save hi-score
        const auto score = get_score(pimpl_->state.brd);
        auto& hi_score = pimpl_->state.hi_score;
        if(hi_score < score)
        {
            hi_score = score;
            events.push_back(events::hi_score_change{hi_score});
        }
    }
    else
    {
        //move the next input into the input
        pimpl_->state.input_tiles = pimpl_->state.next_input_tiles;
        events.push_back(events::next_input_insertion{});

        //create a new next input
        events.push_back(pimpl_->generate_next_input());
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
