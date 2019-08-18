/*
Copyright 2018 - 2019 Florian Goujeon

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

    inline
    std::ostream& operator<<(std::ostream& l, const start&)
    {
        return l << "start{}";
    }



    struct next_input_creation
    {
        data_types::tile_pair tiles;
    };

    inline
    std::ostream& operator<<(std::ostream& l, const next_input_creation& r)
    {
        l << "next_input_creation";
        l << "{";
        l << "tiles: " << r.tiles[0].value << ", " << r.tiles[1].value;
        l << "}";
        return l;
    }



    struct next_input_insertion
    {
        unsigned int x_offset;
        unsigned int rotation;
    };

    inline
    std::ostream& operator<<(std::ostream& l, const next_input_insertion& r)
    {
        l << "next_input_insertion";
        l << "{";
        l << "x_offset: " << r.x_offset << ", ";
        l << "rotation: " << r.rotation;
        l << "}";
        return l;
    }



    struct input_layout_change
    {
        unsigned int x_offset;
        unsigned int rotation;
    };

    inline
    std::ostream& operator<<(std::ostream& l, const input_layout_change& r)
    {
        l << "input_layout_change";
        l << "{";
        l << "x_offset: " << r.x_offset << ", ";
        l << "rotation: " << r.rotation;
        l << "}";
        return l;
    }



    struct input_insertion
    {
        unsigned int tile0_dst_column_index;
        unsigned int tile0_dst_row_index;
        unsigned int tile1_dst_column_index;
        unsigned int tile1_dst_row_index;
    };

    inline
    std::ostream& operator<<(std::ostream& l, const input_insertion& r)
    {
        l << "input_insertion";
        l << "{";
        l << "tile0_dst_column_index: " << r.tile0_dst_column_index << ", ";
        l << "tile0_dst_row_index: " << r.tile0_dst_row_index << ", ";
        l << "tile1_dst_column_index: " << r.tile1_dst_column_index << ", ";
        l << "tile1_dst_row_index: " << r.tile1_dst_row_index;
        l << "}";
        return l;
    }



    struct tile_drop
    {
        data_types::tile_drop_list drops;
    };

    inline
    std::ostream& operator<<(std::ostream& l, const tile_drop& r)
    {
        l << "tile_drop";
        l << "{";
        l << "drops: {";
        {
            auto first = true;
            for(const auto& drop: r.drops)
            {
                if(!first) l << ", ";
                l << drop;
                first = false;
            }
        }
        l << "}";
        l << "}";
        return l;
    }



    struct tile_merge
    {
        data_types::tile_merge_list merges;
    };

    inline
    std::ostream& operator<<(std::ostream& l, const tile_merge& r)
    {
        l << "tile_merge";
        l << "{";
        l << "merges: {";
        {
            auto first = true;
            for(const auto& merge: r.merges)
            {
                if(!first) l << ", ";
                l << merge;
                first = false;
            }
        }
        l << "}";
        l << "}";
        return l;
    }



    struct score_change
    {
        unsigned int score;
    };

    inline
    std::ostream& operator<<(std::ostream& l, const score_change& r)
    {
        l << "score_change";
        l << "{";
        l << "score: " << r.score;
        l << "}";
        return l;
    }



    struct hi_score_change
    {
        unsigned int score;
    };

    inline
    std::ostream& operator<<(std::ostream& l, const hi_score_change& r)
    {
        l << "hi_score_change";
        l << "{";
        l << "score: " << r.score;
        l << "}";
        return l;
    }



    struct end_of_game{};

    inline
    std::ostream& operator<<(std::ostream& l, const end_of_game&)
    {
        return l << "end_of_game{}";
    }
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

} //namespace libgame

#endif
