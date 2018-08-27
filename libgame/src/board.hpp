#ifndef BOARD_HPP
#define BOARD_HPP

#include <libgame/events.hpp>
#include <libgame/data_types.hpp>
#include <vector>
#include <memory>

namespace libgame
{

class board_input;

class board
{
	public:
		static const unsigned int column_count = 6;
		static const unsigned int row_count = 10;
        using grid_t = tile_grid<column_count, row_count>;

		board();

		const grid_t&
		tile_grid() const
		{
			return tile_grid_;
		}

		bool
		is_game_over() const;

		unsigned int
		get_highest_unlocked_element_index() const
		{
			return highest_unlocked_element_index_;
		}

        unsigned int
        get_score() const;

        std::vector<std::vector<event>>
        drop_input(const board_input& in);

	private:
        events::input_insertion
		insert_input(const board_input& in);

        std::vector<event>
		make_tiles_fall();

        std::vector<event>
		transmute_elements();

		enum class selection_state
		{
			UNSELECTED,
			VISITED,
			SELECTED
		};

		typedef array2d<selection_state, column_count, row_count> selection_t;

		void
		select_elements
		(
			const unsigned int element_value,
			const unsigned int column_index,
			const unsigned int row_index,
			selection_t& selection,
			unsigned int& selection_size
		);

	private:
		grid_t tile_grid_;
		unsigned int highest_unlocked_element_index_;
};

} //namespace libgame

#endif
