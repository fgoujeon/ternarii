#ifndef BOARD_INPUT_HPP
#define BOARD_INPUT_HPP

#include <libgame/events.hpp>
#include <libgame/data_types.hpp>
#include <array>
#include <vector>
#include <memory>

namespace libgame
{

class board_input
{
	public:
		static const unsigned int column_count = 6;
        using items_t = std::array<element, 2>;

	public:
        board_input(const board_next_input_t& items);

        const board_next_input_t&
		get_items() const
        {
            return items_;
        }

        event
		set_items(const board_next_input_t& rhs);

        unsigned int
        get_x_offset() const
        {
            return x_offset_;
        }

        std::vector<event>
		shift_left();

        std::vector<event>
		shift_right();

        unsigned int
        get_rotation() const
        {
            return rotation_;
        }

        std::vector<event>
		rotate();

    private:
        std::vector<event>
        apply();

	private:
        items_t items_;
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
