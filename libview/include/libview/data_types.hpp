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

#ifndef LIBVIEW_DATA_TYPES_HPP
#define LIBVIEW_DATA_TYPES_HPP

#include <functional>
#include <optional>
#include <array>
#include <ostream>

namespace libview
{

struct item
{
    item(unsigned int value = 0):
        value(value)
    {
    }

    unsigned int value;
};

using opt_item = std::optional<item>;

template<size_t Size0, size_t Size1>
using item_array = std::array<std::array<opt_item, Size1>, Size0>;

using next_input_item_array = std::array<opt_item, 2>;
using input_item_array = std::array<opt_item, 2>;
using board_item_array = item_array<6, 10>;



struct tile_coordinate
{
    unsigned int x;
    unsigned int y;
};

inline
std::ostream& operator<<(std::ostream& l, const tile_coordinate& r)
{
    l << "tile_coordinate";
    l << "{";
    l << "x: " << r.x << ", ";
    l << "y: " << r.y;
    l << "}";
    return l;
}

} //namespace libview

#endif
