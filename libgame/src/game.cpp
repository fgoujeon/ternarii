/*
Copyright 2018 - 2019 Florian Goujeon

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
    class random_tile_generator
    {
        public:
            random_tile_generator():
                gen_(rd_()),
                dis_(0, 4) //with this, we have 0.66% chance to get a 10
            {
            }

            //return random value from 0 to max
            int generate(const int max)
            {
                //generate random number with normal distribution
                const auto real_val = dis_(gen_);

                //remove negative values
                const auto positive_real_val = std::abs(real_val);

                //discretize
                const auto natural_val = static_cast<int>(positive_real_val);

                //stay inside [0, max]
                return natural_val % (max + 1);
            }

        private:
            std::random_device rd_;
            std::mt19937 gen_;
            std::normal_distribution<double> dis_;
    };
}

struct game::impl
{
    impl(const int hi_score):
        board_(hi_score)
    {
    }

    events::next_input_creation generate_next_input()
    {
        const auto highest_tile_value = board_.get_highest_tile_value();
        const auto max_value = std::clamp(highest_tile_value, 2, 9);
        next_input_ = data_types::tile_pair
        {
            data_types::tile{rand.generate(max_value)},
            data_types::tile{rand.generate(max_value)}
        };

        return events::next_input_creation
        {
            next_input_[0],
            next_input_[1]
        };
    }

    random_tile_generator rand;
    board board_;
    board_input input_;
    data_types::tile_pair next_input_;
};

game::game(const int hi_score):
    pimpl_(std::make_unique<impl>(hi_score))
{
}

game::~game() = default;

int game::get_score() const
{
    return pimpl_->board_.get_score();
}

const data_types::tile_pair& game::get_next_input_tiles() const
{
    return pimpl_->next_input_;
}

const data_types::input_state& game::get_input_state() const
{
    return pimpl_->input_.get_state();
}

const data_types::board_tile_grid& game::get_board_tiles() const
{
    return pimpl_->board_.tile_grid();
}

bool game::is_game_over() const
{
    return pimpl_->board_.is_game_over();
}

event_list game::start()
{
    event_list events;

    pimpl_->board_.clear();

    events.push_back(events::start{});
    events.push_back(events::score_change{0});

    events.push_back(pimpl_->generate_next_input());
    events.push_back(pimpl_->input_.set_tiles(pimpl_->next_input_)); //insert next input
    events.push_back(pimpl_->generate_next_input());

    return events;
}

event_list game::shift_input_left()
{
    if(!is_game_over())
    {
        return pimpl_->input_.shift_left();
    }
    else
    {
        return {};
    }
}

event_list game::shift_input_right()
{
    if(!is_game_over())
    {
        return pimpl_->input_.shift_right();
    }
    else
    {
        return {};
    }
}

event_list game::rotate_input()
{
    if(!is_game_over())
    {
        return pimpl_->input_.rotate();
    }
    else
    {
        return {};
    }
}

event_list game::drop_input()
{
    event_list events;

    if(!is_game_over())
    {
        //drop the input
        const auto& board_events = pimpl_->board_.drop_input(pimpl_->input_);
        for(const auto& event: board_events)
        {
            events.push_back(event);
        }

        if(!is_game_over())
        {
            //move the next input into the input
            events.push_back(pimpl_->input_.set_tiles(pimpl_->next_input_));

            //create a new next input
            events.push_back(pimpl_->generate_next_input());
        }
    }

    if(is_game_over())
    {
        events.push_back(events::end_of_game{});
    }

    return events;
}

} //namespace
