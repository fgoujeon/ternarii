#ifndef BOARD_INPUT_HPP
#define BOARD_INPUT_HPP

#include <libgame/game_change.hpp>
#include <libgame/item_grid.hpp>
#include <libgame/array2d.hpp>
#include <libgame/board_next_input.hpp>
#include <libgame/element.hpp>
#include <array>
#include <vector>
#include <memory>

namespace libgame
{

class board_input
{
	public:
		static const unsigned int column_count = 6;
		static const unsigned int row_count = 2;
        using grid_t = item_grid_t<column_count, row_count>;
        using items_t = std::array<std::shared_ptr<element>, 2>;

		const grid_t&
		item_grid() const
		{
			return item_grid_;
		}

        const board_next_input_t&
		get_items() const
        {
            return items_;
        }

        std::vector<game_change_t>
		set_items(const board_next_input_t& rhs);

        unsigned int
        get_x_offset() const
        {
            return x_offset_;
        }

        std::vector<game_change_t>
		shift_left();

        std::vector<game_change_t>
		shift_right();

        unsigned int
        get_rotation() const
        {
            return rotation_;
        }

        std::vector<game_change_t>
		rotate();

    private:
        std::vector<game_change_t>
        apply();

	private:
        items_t items_;
		grid_t item_grid_;
		unsigned int x_offset_;

        //rotation = 0:
        //  --
        //  01
        //rotation = 1:
        //  0-
        //  1-
        //rotation = 2:
        //  --
        //  10
        //rotation = 3:
        //  1-
        //  0-
        unsigned int rotation_;
};

} //namespace libgame

#endif
