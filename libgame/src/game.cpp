/*
Copyright 2018 Florian Goujeon

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
#include <cmath>
#include <cstdlib>
#include <cassert>

namespace libgame
{

namespace
{
    //return random value from 0 to max
    unsigned int random_value(const unsigned int max)
    {
        const auto random = static_cast<double>(std::rand() - 1) / RAND_MAX; //[0, 1)
        return (max + 1) * std::pow(random, 2);
    }

    tile generate_new_tile(const unsigned int max_value)
    {
        return tile{random_value(max_value)};
    }

    tile_pair generate_next_input(const unsigned int highest_unlocked_element_value)
    {
        return
        {
            generate_new_tile(highest_unlocked_element_value - 1),
            generate_new_tile(highest_unlocked_element_value - 1)
        };
    }
}

struct game::impl
{
    impl():
        next_input_(generate_next_input(get_highest_unlocked_element_index())),
        input_(generate_next_input(get_highest_unlocked_element_index()))
    {
    }

    unsigned int get_highest_unlocked_element_index() const
    {
        return board_.get_highest_tile_ever();
    }

    board board_;
    board_input input_;
    tile_pair next_input_;
};

game::game():
	pimpl_(std::make_unique<impl>())
{
}

game::~game() = default;

unsigned int game::get_score() const
{
    return pimpl_->board_.get_score();
}

const tile_pair& game::get_next_input_tiles() const
{
    return pimpl_->next_input_;
}

const tile_pair& game::get_input_tiles() const
{
    return pimpl_->input_.get_tiles();
}

const board_tile_grid& game::get_board_tiles() const
{
    return pimpl_->board_.tile_grid();
}

bool game::is_game_over() const
{
    return pimpl_->board_.is_game_over();
}

unsigned int game::get_input_x_offset() const
{
    return pimpl_->input_.get_x_offset();
}

unsigned int game::get_input_rotation() const
{
    return pimpl_->input_.get_rotation();
}

event_list game::shift_input_left()
{
    return pimpl_->input_.shift_left();
}

event_list game::shift_input_right()
{
    return pimpl_->input_.shift_right();
}

event_list game::rotate_input()
{
    return pimpl_->input_.rotate();
}

event_list game::drop_input()
{
    event_list events;

    if(!is_game_over())
    {
        //drop the input
        const auto& board_events = pimpl_->board_.drop_input(pimpl_->input_);
        for(const auto& event: board_events)
            events.push_back(event);

        if(!is_game_over())
        {
            //move the next input into the input
            events.push_back(pimpl_->input_.set_tiles(pimpl_->next_input_));

            //create a new next input
            pimpl_->next_input_ = generate_next_input(pimpl_->get_highest_unlocked_element_index());
            events.push_back
            (
                events::next_input_creation
                {
                    pimpl_->next_input_[0],
                    pimpl_->next_input_[1]
                }
            );
        }
    }

    if(is_game_over())
        events.push_back(events::end_of_game{});

    return events;
}

} //namespace libgame
