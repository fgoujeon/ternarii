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

#ifndef CONVERSION_HPP
#define CONVERSION_HPP

#include <libview/data_types.hpp>
#include <libgame/data_types.hpp>

namespace conversion
{

libview::tile_coordinate to_view(const libgame::tile_coordinate& from);

libview::tile_drop to_view(const libgame::tile_drop& from);

template<class T>
auto to_view(const std::vector<T>& from)
{
    using to_item_t = decltype(to_view(std::declval<T>()));
    std::vector<to_item_t> to;
    for(const auto& from_item: from)
        to.push_back(to_view(from_item));
    return to;
}

} //namespace conversion

#endif
