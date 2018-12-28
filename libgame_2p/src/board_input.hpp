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

#ifndef BOARD_INPUT_HPP
#define BOARD_INPUT_HPP

#include <libgame_2p/events.hpp>
#include <libgame_2p/data_types.hpp>
#include <array>
#include <vector>
#include <memory>

namespace libgame_2p
{

class board_input
{
    public:
        static const unsigned int column_count = 6;
        using tiles_t = std::array<data_types::tile, 2>;

    public:
        board_input();

        const data_types::tile_pair& get_tiles() const
        {
            return tiles_;
        }

        event set_tiles(const data_types::tile_pair& rhs);

        unsigned int get_x_offset() const
        {
            return x_offset_;
        }

        std::vector<event> shift_left();

        std::vector<event> shift_right();

        unsigned int get_rotation() const
        {
            return rotation_;
        }

        std::vector<event> rotate();

    private:
        std::vector<event> apply();

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

} //namespace libgame_2p

#endif
