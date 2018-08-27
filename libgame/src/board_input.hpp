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
        using tiles_t = std::array<tile, 2>;

	public:
        board_input(const tile_pair& tiles);

        const tile_pair&
		get_tiles() const
        {
            return tiles_;
        }

        event
		set_tiles(const tile_pair& rhs);

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
        tiles_t tiles_;
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
