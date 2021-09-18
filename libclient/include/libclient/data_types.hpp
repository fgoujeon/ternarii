/*
Copyright 2018 - 2021 Florian Goujeon

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

#ifndef LIBCLIENT_DATA_TYPES_HPP
#define LIBCLIENT_DATA_TYPES_HPP

#include <vector>

namespace libclient::data_types
{

struct move
{
    int column_offset = 0;
    int rotation = 0;
    unsigned long long next_input_random_number = 0;
};

std::ostream& operator<<(std::ostream& l, const move& r);



struct game
{
    int id = 0;
    unsigned long long first_input_random_number = 0;
    unsigned long long first_next_input_random_number = 0;
    std::vector<move> moves;
};

std::ostream& operator<<(std::ostream& l, const game& r);

} //namespace

#endif
