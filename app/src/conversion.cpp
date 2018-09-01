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

#include "conversion.hpp"

namespace conversion
{

libview::tile_coordinate to_view(const libgame::tile_coordinate& from)
{
    auto to = libview::tile_coordinate{};
    to.x = from.x;
    to.y = from.y;
    return to;
}

libview::tile_drop to_view(const libgame::tile_drop& from)
{
    auto to = libview::tile_drop{};
    to.column_index = from.column_index;
    to.src_row_index = from.src_row_index;
    to.dst_row_index = from.dst_row_index;
    return to;
}

} //namespace conversion
