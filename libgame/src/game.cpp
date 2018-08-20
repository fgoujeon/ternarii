#include <libgame/game.hpp>
#include "board.hpp"
#include "board_input.hpp"
#include <algorithm>
#include <cstdlib>
#include <cassert>

namespace libgame
{

namespace
{
    std::shared_ptr<element>
    generate_new_element(const unsigned int highest_unlocked_element_value)
    {
        const double cumulated_probabilities[11] =
        {
            /* 0*/ 0.175416,
            /* 1*/ 0.34315,
            /* 2*/ 0.507042,
            /* 3*/ 0.668374,
            /* 4*/ 0.771447,
            /* 5*/ 0.8418695,
            /* 6*/ 0.9052498,
            /* 7*/ 0.9475033,
            /* 8*/ 0.9827146,
            /* 9*/ 0.9929579,
            /*10*/ 1
        };

        const unsigned int max_element_index =
            std::min(highest_unlocked_element_value, 10u) //10: highest element that can be generated on the input
        ;
        assert(max_element_index < 11);
        const unsigned int random = rand() * cumulated_probabilities[max_element_index];

        for(unsigned int i = 0; i < max_element_index; ++i)
        {
            if(random <= cumulated_probabilities[i] * RAND_MAX)
            {
                return std::make_shared<element>(element{i});
            }
        }
        return std::make_shared<element>(element{max_element_index});
    }

    board_next_input_t
    generate_next_input(const unsigned int highest_unlocked_element_value)
    {
        return
        {
            generate_new_element(highest_unlocked_element_value),
            generate_new_element(highest_unlocked_element_value)
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

const board_next_input_t& game::next_input_items() const
{
    return pimpl_->next_input_;
}

const board_next_input_t& game::get_input_items() const
{
    return pimpl_->input_.get_items();
}

const board_grid_t& game::board_items() const
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

void game::shift_input_left()
{
    //change_signal_(input_.shift_left());
    pimpl_->input_.shift_left();
}

void game::shift_input_right()
{
    //change_signal_(input_.shift_right());
    pimpl_->input_.shift_right();
}

void game::rotate_input()
{
    //change_signal_(input_.rotate());
    pimpl_->input_.rotate();
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
