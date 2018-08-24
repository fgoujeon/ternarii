#ifndef GAME_HPP
#define GAME_HPP

#include "board_next_input.hpp"
#include "board_grid.hpp"
#include "game_change.hpp"
#include <vector>

namespace libgame
{

struct game
{
	public:
		game();

		~game();

        unsigned int get_score() const;

		const board_next_input_t& next_input_items() const;

		const board_next_input_t& get_input_items() const;

		const board_grid_t& board_items() const;

		bool is_game_over() const;

		unsigned int get_highest_unlocked_element_index() const;

        unsigned int get_input_x_offset() const;

        unsigned int get_input_rotation() const;

		game_change_list shift_input_left();

		game_change_list shift_input_right();

		game_change_list rotate_input();

		void drop_input();

	private:
        struct impl;
        std::unique_ptr<impl> pimpl_;
};

} //namespace libgame

#endif
