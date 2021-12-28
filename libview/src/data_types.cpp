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

#include "data_types.hpp"
#include <libres.hpp>

namespace libview::data_types
{

std::string_view get_pretty_name(const stage s)
{
    switch(s)
    {
        case stage::granite_cave:       return "GRANITE CAVE";
        case stage::math_classroom:     return "MATH CLASSROOM";
        case stage::nullifier_room:     return "NULLIFIER ROOM";
        case stage::purity_chapel:      return "PURITY CHAPEL";
        case stage::triplet_pines_mall: return "TRIPLET PINES MALL";
        case stage::waterfalls:         return "WATERFALLS";
    }
    return "";
}

std::optional<std::filesystem::path> get_image(const stage s)
{
    switch(s)
    {
#define CASE(NAME) \
    case stage::NAME: \
        return libres::images::background_##NAME;

        CASE(granite_cave)
        CASE(math_classroom)
        CASE(nullifier_room)
        CASE(purity_chapel)
        CASE(triplet_pines_mall)
        CASE(waterfalls)

#undef CASE
    }
    return std::nullopt;
}

} //namespace
