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
#include "board.hpp"
#include "board_input.hpp"
#include <algorithm>
#include <random>
#include <cmath>
#include <cassert>

namespace libgame
{

namespace
{
    struct random_number_generator
    {
        std::random_device rd;
        std::mt19937 gen{rd()};
    };

    class random_tile_generator
    {
        private:
            using distribution = std::normal_distribution<double>;

        public:
            random_tile_generator(random_number_generator& rng):
                rng_(rng)
            {
            }

            //return random value from 0 to max
            int generate(const int max, const double standard_deviation)
            {
                //generate random number with normal distribution
                const auto real_val = dis_(rng_.gen, distribution::param_type{0, standard_deviation});

                //remove negative values
                const auto positive_real_val = std::abs(real_val);

                //discretize
                const auto natural_val = static_cast<int>(positive_real_val);

                //stay inside [0, max]
                return natural_val % (max + 1);
            }

        private:
            random_number_generator& rng_;
            distribution dis_;
    };

    class random_tile_matrix_generator
    {
        private:
            using distribution = std::discrete_distribution<int>;

        public:
            data_types::input_tile_array generate(const int max, const double standard_deviation)
            {
                const auto count = dis_(rng_.gen);

                switch(count)
                {
                    default:
                    case 0:
                        return
                        {
                            data_types::tiles::number{gen_.generate(max, standard_deviation)},
                            data_types::tiles::number{gen_.generate(max, standard_deviation)}
                        };
                    case 1: return {data_types::tiles::column_nullifier{}};
                    case 2: return {data_types::tiles::row_nullifier{}};
                    case 3: return {data_types::tiles::number_nullifier{}};
                }
            }

        private:
            random_number_generator rng_;
            distribution dis_{97, 1, 1, 1};
            random_tile_generator gen_{rng_};
    };
}

struct game::impl
{
    impl() = default;

    impl(const data_types::game_state& s):
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
        const auto highest_tile_value = board_.get_highest_tile_value();
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
                static_cast<double>(board::authorized_cell_count - board_.get_free_cell_count()) /
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
        state.next_input_tiles = rand.generate(max_value, sd);

        return events::next_input_creation
        {
            state.next_input_tiles
        };
    }

    random_tile_matrix_generator rand;
    data_types::game_state state;
    board board_{state.board_tiles};
    board_input input_{state.input_tiles};
};

game::game():
    pimpl_(std::make_unique<impl>())
{
}

game::game(const data_types::game_state& state):
    pimpl_(std::make_unique<impl>(state))
{
}

game::~game() = default;

const data_types::game_state& game::get_state() const
{
    return pimpl_->state;
}

int game::get_score() const
{
    return pimpl_->board_.get_score();
}

int game::get_hi_score() const
{
    return pimpl_->state.hi_score;
}

const data_types::input_tile_array& game::get_next_input_tiles() const
{
    return pimpl_->state.next_input_tiles;
}

const data_types::input_tile_array& game::get_input_tiles() const
{
    return pimpl_->state.input_tiles;
}

const data_types::input_layout& game::get_input_layout() const
{
    return pimpl_->input_.get_layout();
}

const data_types::board_tile_array& game::get_board_tiles() const
{
    return pimpl_->board_.tile_array();
}

bool game::is_game_over() const
{
    return pimpl_->board_.is_game_over();
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
    if(!is_game_over())
    {
        pimpl_->input_.shift_left(events);
    }
}

void game::shift_input_right(event_list& events)
{
    if(!is_game_over())
    {
        pimpl_->input_.shift_right(events);
    }
}

void game::rotate_input(event_list& events)
{
    if(!is_game_over())
    {
        pimpl_->input_.rotate(events);
    }
}

void game::drop_input_tiles(event_list& events)
{
    if(is_game_over())
    {
        return;
    }

    //drop the input
    pimpl_->board_.drop_input_tiles(pimpl_->input_, events);

    if(is_game_over())
    {
        events.push_back(events::end_of_game{});

        //Save hi-score
        auto& hi_score = pimpl_->state.hi_score;
        if(hi_score < get_score())
        {
            hi_score = get_score();
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
