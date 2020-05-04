/*
Copyright 2018 - 2020 Florian Goujeon

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

#ifndef LIBGAME_EVENT_HPP
#define LIBGAME_EVENT_HPP

#include "data_types.hpp"
#include <variant>
#include <vector>
#include <memory>
#include <array>
#include <ostream>

namespace libgame
{

namespace events
{
    struct start{};

    std::ostream& operator<<(std::ostream& l, const start&);



    struct next_input_creation
    {
        data_types::input_tile_array tiles;
    };

    std::ostream& operator<<(std::ostream& l, const next_input_creation& r);



    struct next_input_insertion
    {
        data_types::input_layout layout;
    };

    std::ostream& operator<<(std::ostream& l, const next_input_insertion& r);



    struct input_layout_change
    {
        data_types::input_layout layout;
    };

    std::ostream& operator<<(std::ostream& l, const input_layout_change& r);



    struct input_insertion
    {
        data_types::tile_coordinate_list dst_coordinates;
    };

    std::ostream& operator<<(std::ostream& l, const input_insertion& r);



    struct tile_drop
    {
        data_types::tile_drop_list drops;
    };

    std::ostream& operator<<(std::ostream& l, const tile_drop& r);



    struct tile_merge
    {
        data_types::tile_merge_list merges;
    };

    std::ostream& operator<<(std::ostream& l, const tile_merge& r);



    struct score_change
    {
        int score = 0;
    };

    std::ostream& operator<<(std::ostream& l, const score_change& r);



    struct hi_score_change
    {
        int score = 0;
    };

    std::ostream& operator<<(std::ostream& l, const hi_score_change& r);



    struct end_of_game{};

    std::ostream& operator<<(std::ostream& l, const end_of_game&);
}

using event = std::variant
<
    events::start,
    events::next_input_creation,
    events::next_input_insertion,
    events::input_layout_change,
    events::input_insertion,
    events::tile_drop,
    events::tile_merge,
    events::score_change,
    events::hi_score_change,
    events::end_of_game
>;

using event_list = std::vector<event>;

} //namespace

#endif
