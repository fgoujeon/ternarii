#ifndef GAME_HPP
#define GAME_HPP

#include "events.hpp"
#include "data_types.hpp"
#include <memory>

namespace libgame
{

struct game
{
	public:
		game();

		~game();

        unsigned int get_score() const;

		const tile_pair& get_next_input_tiles() const;

		const tile_pair& get_input_tiles() const;

		const board_tile_grid& get_board_tiles() const;

		bool is_game_over() const;

		unsigned int get_highest_unlocked_element_index() const;

        unsigned int get_input_x_offset() const;

        unsigned int get_input_rotation() const;

		event_list shift_input_left();

		event_list shift_input_right();

		event_list rotate_input();

		event_list drop_input();

	private:
        struct impl;
        std::unique_ptr<impl> pimpl_;
};

} //namespace libgame

#endif
