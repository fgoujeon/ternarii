/*
Copyright 2018 - 2022 Florian Goujeon

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

#ifndef LIBGAME_INPUT_GENERATORS_HPP
#define LIBGAME_INPUT_GENERATORS_HPP

#include <libgame/data_types.hpp>

namespace libgame
{

struct abstract_input_generator
{
    virtual ~abstract_input_generator() = default;

    virtual data_types::input_tile_matrix generate
    (
        const int board_highest_tile_value,
        const int board_tile_count
    ) = 0;
};

abstract_input_generator& get_input_generator(data_types::stage stage);

} //namespace

#endif
