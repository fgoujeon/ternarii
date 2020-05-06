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

#include <libgame/events.hpp>
#include <libutil/streamable.hpp>

namespace libgame::events
{

std::ostream& operator<<(std::ostream& l, const start&)
{
    return l << "start{}";
}

std::ostream& operator<<(std::ostream& l, const next_input_creation& r)
{
    l << "next_input_creation";
    l << "{";
    l << "tiles: " << libutil::streamable{r.tiles};
    l << "}";
    return l;
}

std::ostream& operator<<(std::ostream& l, const next_input_insertion& r)
{
    l << "next_input_insertion";
    l << "{";
    l << "layout: " << r.layout;
    l << "}";
    return l;
}

std::ostream& operator<<(std::ostream& l, const input_layout_change& r)
{
    l << "input_layout_change";
    l << "{";
    l << "layout: " << r.layout;
    l << "}";
    return l;
}

std::ostream& operator<<(std::ostream& l, const input_tile_drop& r)
{
    l << "input_tile_drop";
    l << "{";
    l << "drops: " << libutil::streamable{r.drops};
    l << "}";
    return l;
}

std::ostream& operator<<(std::ostream& l, const board_tile_drop& r)
{
    l << "board_tile_drop";
    l << "{";
    l << "drops: " << libutil::streamable{r.drops};
    l << "}";
    return l;
}

std::ostream& operator<<(std::ostream& l, const vertical_dynamite_tile_explosion& r)
{
    l << "vertical_dynamite_tile_explosion";
    l << "{";
    l << "explosions: " << libutil::streamable{r.explosions};
    l << "}";
    return l;
}

std::ostream& operator<<(std::ostream& l, const tile_merge& r)
{
    l << "tile_merge";
    l << "{";
    l << "merges: " << libutil::streamable{r.merges};
    l << "}";
    return l;
}

std::ostream& operator<<(std::ostream& l, const score_change& r)
{
    l << "score_change";
    l << "{";
    l << "score: " << r.score;
    l << "}";
    return l;
}

std::ostream& operator<<(std::ostream& l, const hi_score_change& r)
{
    l << "hi_score_change";
    l << "{";
    l << "score: " << r.score;
    l << "}";
    return l;
}

std::ostream& operator<<(std::ostream& l, const end_of_game&)
{
    return l << "end_of_game{}";
}

} //namespace
