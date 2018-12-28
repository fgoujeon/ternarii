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

#include <libgame_2p/game.hpp>
#include "board.hpp"
#include "board_input.hpp"
#include <algorithm>
#include <random>
#include <cmath>
#include <cassert>

namespace libgame_2p
{

namespace
{
    using board_next_input = data_types::tile_pair;

    class random_tile_generator
    {
        public:
            random_tile_generator():
                gen_(rd_()),
                dis_(0, 4) //with this, we have 0.66% chance to get a 10
            {
            }

            //return random value from 0 to max
            unsigned int generate(const unsigned int max)
            {
                //generate random number with normal distribution
                const auto real_val = dis_(gen_);

                //remove negative values
                const auto positive_real_val = std::abs(real_val);

                //discretize
                const auto natural_val = static_cast<unsigned int>(positive_real_val);

                //stay inside [0, max]
                return natural_val % (max + 1);
            }

        private:
            std::random_device rd_;
            std::mt19937 gen_;
            std::normal_distribution<double> dis_;
    };

    constexpr auto player_count = 2;
}

struct game::impl
{
    data_types::tile make_random_tile()
    {
        auto highest_tile_value = 2u;
        for(auto& board: boards_)
        {
            highest_tile_value = std::max(highest_tile_value, board.get_highest_tile_value());
        }
        return data_types::tile{rand.generate(highest_tile_value)};
    }

    events::next_input_creation generate_next_input(const int player_index)
    {
        const auto highest_tile_value = boards_[player_index].get_highest_tile_value();
        const auto max_value = std::max(highest_tile_value, 2u);
        next_inputs_[player_index] = data_types::tile_pair
        {
            data_types::tile{rand.generate(max_value)},
            data_types::tile{rand.generate(max_value)}
        };

        return events::next_input_creation
        {
            player_index,
            next_inputs_[player_index][0],
            next_inputs_[player_index][1]
        };
    }

    events::next_input_creation generate_current_player_next_input()
    {
        return generate_next_input(current_player_index_);
    }

    const board& get_current_player_board() const
    {
        return boards_[current_player_index_];
    }

    board& get_current_player_board()
    {
        return boards_[current_player_index_];
    }

    const board_input& get_current_player_input() const
    {
        return inputs_[current_player_index_];
    }

    board_input& get_current_player_input()
    {
        return inputs_[current_player_index_];
    }

    const board_next_input& get_current_player_next_input() const
    {
        return next_inputs_[current_player_index_];
    }

    board_next_input& get_current_player_next_input()
    {
        return next_inputs_[current_player_index_];
    }

    void select_next_player()
    {
        current_player_index_ = (current_player_index_ + 1) % player_count;
    }

    random_tile_generator rand;
    std::array<board, 2> boards_ = {board{0}, board{1}};
    std::array<board_input, 2> inputs_ = {board_input{0}, board_input{1}};
    std::array<board_next_input, 2> next_inputs_;
    data_types::tile_pool tile_pool_;
    int current_player_index_ = 0;
};

game::game():
    pimpl_(std::make_unique<impl>())
{
}

game::~game() = default;

unsigned int game::get_score() const
{
    return pimpl_->get_current_player_board().get_score();
}

const data_types::tile_pair& game::get_next_input_tiles() const
{
    return pimpl_->get_current_player_next_input();
}

const data_types::tile_pair& game::get_input_tiles() const
{
    return pimpl_->get_current_player_input().get_tiles();
}

const data_types::board_tile_grid& game::get_board_tiles() const
{
    return pimpl_->get_current_player_board().tile_grid();
}

bool game::is_game_over() const
{
    return pimpl_->get_current_player_board().is_game_over();
}

unsigned int game::get_input_x_offset() const
{
    return pimpl_->get_current_player_input().get_x_offset();
}

unsigned int game::get_input_rotation() const
{
    return pimpl_->get_current_player_input().get_rotation();
}

event_list game::start()
{
    event_list events;

    pimpl_->current_player_index_ = 0;

    //clear boards
    for(auto& board: pimpl_->boards_)
        board.clear();

    //initialize tile pool
    for(auto& tile_column: pimpl_->tile_pool_)
    {
        for(auto& tile: tile_column)
        {
            tile = pimpl_->make_random_tile();
        }
    }

    events.push_back(events::start{});

    for(auto player_index = 0; player_index < player_count; ++player_index)
    {
        events.push_back
        (
            events::score_change
            {
                player_index,
                0
            }
        );

        events.push_back(pimpl_->generate_next_input(player_index));
    }

    events.push_back(pimpl_->inputs_[0].set_tiles(pimpl_->next_inputs_[0])); //insert next input
    events.push_back(pimpl_->generate_next_input(0));

    events.push_back
    (
        events::tile_pool_change
        {
            pimpl_->tile_pool_
        }
    );

    return events;
}

event_list game::shift_input_left()
{
    return pimpl_->get_current_player_input().shift_left();
}

event_list game::shift_input_right()
{
    return pimpl_->get_current_player_input().shift_right();
}

event_list game::rotate_input()
{
    return pimpl_->get_current_player_input().rotate();
}

event_list game::drop_input()
{
    event_list events;

    if(!is_game_over())
    {
        //drop the input
        const auto& board_events = pimpl_->get_current_player_board().drop_input(pimpl_->get_current_player_input());
        for(const auto& event: board_events)
            events.push_back(event);

        if(!is_game_over())
        {
            pimpl_->select_next_player();

            //move the next input into the input
            events.push_back(pimpl_->get_current_player_input().set_tiles(pimpl_->get_current_player_next_input()));

            //create a new next input
            events.push_back(pimpl_->generate_current_player_next_input());
        }
    }

    if(is_game_over())
        events.push_back(events::end_of_game{});

    return events;
}

} //namespace libgame_2p
