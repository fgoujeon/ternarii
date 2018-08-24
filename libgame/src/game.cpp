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

    std::shared_ptr<element>
    generate_new_element(const unsigned int max_value)
    {
        return std::make_shared<element>(random_value(max_value));
    }

    board_next_input_t
    generate_next_input(const unsigned int highest_unlocked_element_value)
    {
        return
        {
            generate_new_element(highest_unlocked_element_value - 1),
            generate_new_element(highest_unlocked_element_value - 1)
        };
    }
}

struct game::impl
{
    impl():
        next_input_(generate_next_input(get_highest_unlocked_element_index()))
    {
        //change_signal_(input_.set_items(generate_next_input(get_highest_unlocked_element_index())));
        input_.set_items(generate_next_input(get_highest_unlocked_element_index()));
    }

    unsigned int get_highest_unlocked_element_index() const
    {
        return board_.get_highest_unlocked_element_index();
    }

    unsigned int highest_unlocked_element_;
    board board_;
    board_input input_;
    board_next_input_t next_input_;
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

const board_next_input_t& game::get_next_input_items() const
{
    return pimpl_->next_input_;
}

const board_next_input_t& game::get_input_items() const
{
    return pimpl_->input_.get_items();
}

const board_grid_t& game::get_board_items() const
{
    return pimpl_->board_.item_grid();
}

bool game::is_game_over() const
{
    return pimpl_->board_.is_game_over();
}

unsigned int game::get_highest_unlocked_element_index() const
{
    return pimpl_->board_.get_highest_unlocked_element_index();
}

unsigned int game::get_input_x_offset() const
{
    return pimpl_->input_.get_x_offset();
}

unsigned int game::get_input_rotation() const
{
    return pimpl_->input_.get_rotation();
}

game_change_list game::shift_input_left()
{
    //change_signal_(input_.shift_left());
    return pimpl_->input_.shift_left();
}

game_change_list game::shift_input_right()
{
    //change_signal_(input_.shift_right());
    return pimpl_->input_.shift_right();
}

game_change_list game::rotate_input()
{
    //change_signal_(input_.rotate());
    return pimpl_->input_.rotate();
}

void game::drop_input()
{
    if(!is_game_over())
    {
        //change_signal_({board_input_changes::clear{}});

        //drop the input
        const auto& change_sets = pimpl_->board_.drop_input(pimpl_->input_);
        //for(const auto& change_set: change_sets)
        //    change_signal_(change_set);

        if(!is_game_over())
        {
            //move the next input into the input
            auto changes = pimpl_->input_.set_items(pimpl_->next_input_);

            //create a new next input
            pimpl_->next_input_ = generate_next_input(pimpl_->get_highest_unlocked_element_index());
            changes.push_back
            (
                game_changes::next_input_creation
                {
                    pimpl_->next_input_[0],
                    pimpl_->next_input_[1]
                }
            );

            //change_signal_(changes);
        }
    }
}

} //namespace libgame
