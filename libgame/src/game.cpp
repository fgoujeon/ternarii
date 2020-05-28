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

#include <libgame/game.hpp>
#include "input_generators.hpp"
#include "board.hpp"
#include "board_input.hpp"
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
        /*
        We want to generate tiles whose value goes from 0 to the value of the
        highest tile of the board.
        This max value is clamped between 2 (we don't want only 0s at the
        beginning of a game) and 9 (we don't want the player to get too many
        "free" points from the input just by being lucky).
        */
        const auto highest_tile_value = get_highest_tile_value(state.board_tiles);
        const auto max_value = std::clamp(highest_tile_value, 2, 9);

        /*
        Compute the standard deviation (SD) of the normal distribution (whose
        mean is set to 0).
        The lower the SD is, the more likely the generated value is small.
        We want to adjust the SD according to the fill rate of the board:
        - When the board is near empty, the SD is high so that the player can
          quickly grow his tiles.
        - When the board is near full, the SD is low so that the player gets
          the low-value tiles he might be waiting for to unlock his combos.
        */
        const auto sd = [&]
        {
            /*
            Normalized fill rate of the board.
            Value goes from 0.0 (empty) to 1.0 (full).
            */
            const auto fill_rate =
                static_cast<double>(board::authorized_cell_count - get_free_cell_count(state.board_tiles)) /
                board::authorized_cell_count
            ;

            const auto sd_max = 4.0; //SD of empty board
            const auto sd_min = 1.8; //SD of full board
            const auto sd_variable_part = sd_max - sd_min;

            /*
            The higher the exponent is, the "longer" the SD stays at max value.
            We only want the SD to significantly decrease when the board is 3/4
            full.
            */
            const auto fill_rate_pow = std::pow(fill_rate, 6);

            return sd_min + sd_variable_part * (1.0 - fill_rate_pow);
        }();

        //Generate a new input
        state.next_input_tiles = input_gen.generate(max_value, sd);

        return events::next_input_creation
        {
            state.next_input_tiles
        };
    }

    abstract_input_generator& input_gen;
    data_types::stage_state state;
    board board_{state.board_tiles};
    board_input input_{state.input_tiles};
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

int game::get_hi_score() const
{
    return pimpl_->state.hi_score;
}

const data_types::input_tile_matrix& game::get_next_input_tiles() const
{
    return pimpl_->state.next_input_tiles;
}

const data_types::input_tile_matrix& game::get_input_tiles() const
{
    return pimpl_->state.input_tiles;
}

const data_types::input_layout& game::get_input_layout() const
{
    return pimpl_->input_.get_layout();
}

const data_types::board_tile_matrix& game::get_board_tiles() const
{
    return pimpl_->board_.tile_array();
}

void game::get_targeted_tiles(libutil::matrix_coordinate_list& coords) const
{
    return pimpl_->board_.get_targeted_tiles(pimpl_->input_, coords);
}

bool game::is_over() const
{
    return is_overflowed(pimpl_->state.board_tiles);
}

void game::start(event_list& events)
{
    pimpl_->board_.clear();

    events.push_back(events::start{});
    events.push_back(events::score_change{0});

    events.push_back(pimpl_->generate_next_input());
    events.push_back(pimpl_->input_.set_tiles(pimpl_->state.next_input_tiles)); //insert next input
    events.push_back(pimpl_->generate_next_input());
}

void game::shift_input_left(event_list& events)
{
    if(!is_over())
    {
        pimpl_->input_.shift_left(events);
    }
}

void game::shift_input_right(event_list& events)
{
    if(!is_over())
    {
        pimpl_->input_.shift_right(events);
    }
}

void game::rotate_input(event_list& events)
{
    if(!is_over())
    {
        pimpl_->input_.rotate(events);
    }
}

void game::drop_input_tiles(event_list& events)
{
    if(is_over())
    {
        return;
    }

    //drop the input
    pimpl_->board_.drop_input_tiles(pimpl_->input_, events);

    if(is_over())
    {
        events.push_back(events::end_of_game{});

        //Save hi-score
        const auto score = get_score(pimpl_->state.board_tiles);
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
        events.push_back(pimpl_->input_.set_tiles(pimpl_->state.next_input_tiles));

        //create a new next input
        events.push_back(pimpl_->generate_next_input());
    }
}

} //namespace
