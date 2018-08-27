#ifndef LIBGAME_EVENT_HPP
#define LIBGAME_EVENT_HPP

#include "element.hpp"
#include "tile_coordinate.hpp"
#include <variant>
#include <vector>
#include <memory>
#include <array>
#include <ostream>

namespace libgame
{

namespace events
{
    struct next_input_creation
    {
        std::array<element, 2> items;
    };

    inline
    std::ostream& operator<<(std::ostream& l, const next_input_creation& r)
    {
        l << "next_input_creation";
        l << "{";
        l << "items: " << r.items[0].value << ", " << r.items[1].value;
        l << "}";
        return l;
    }



    struct next_input_introduction
    {
        unsigned int x_offset;
        unsigned int rotation;
    };

    inline
    std::ostream& operator<<(std::ostream& l, const next_input_introduction& r)
    {
        l << "next_input_introduction";
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



    struct input_introduction
    {
        unsigned int tile0_dst_column_index;
        unsigned int tile0_dst_row_index;
        unsigned int tile1_dst_column_index;
        unsigned int tile1_dst_row_index;
    };

    inline
    std::ostream& operator<<(std::ostream& l, const input_introduction& r)
    {
        l << "input_introduction";
        l << "{";
        l << "tile0_dst_column_index: " << r.tile0_dst_column_index << ", ";
        l << "tile0_dst_row_index: " << r.tile0_dst_row_index << ", ";
        l << "tile1_dst_column_index: " << r.tile1_dst_column_index << ", ";
        l << "tile1_dst_row_index: " << r.tile1_dst_row_index;
        l << "}";
        return l;
    }



    struct item_drop
    {
        unsigned int column_index;
        unsigned int src_row_index;
        unsigned int dst_row_index;
    };

    inline
    std::ostream& operator<<(std::ostream& l, const item_drop& r)
    {
        l << "item_drop";
        l << "{";
        l << "column_index: " << r.column_index << ", ";
        l << "src_row_index: " << r.src_row_index << ", ";
        l << "dst_row_index: " << r.dst_row_index;
        l << "}";
        return l;
    }



    struct element_transmutation
    {
        std::vector<tile_coordinate> src_tiles;
        tile_coordinate dst_tile;
        unsigned int dst_tile_value;
    };

    inline
    std::ostream& operator<<(std::ostream& l, const element_transmutation& r)
    {
        l << "element_transmutation";
        l << "{";
        l << "src_tiles: {";
        {
            auto first = true;
            for(const auto& coord: r.src_tiles)
            {
                if(!first) l << ", ";
                l << coord;
                first = false;
            }
        }
        l << "}, ";
        l << "dst_tile: " << r.dst_tile << ", ";
        l << "dst_tile_value: " << r.dst_tile_value;
        l << "}";
        return l;
    }



    struct element_unlocking
    {
        unsigned int index;
    };

    inline
    std::ostream& operator<<(std::ostream& l, const element_unlocking& r)
    {
        l << "element_unlocking";
        l << "{";
        l << "index: " << r.index;
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



    struct end_of_game{};

    inline
    std::ostream& operator<<(std::ostream& l, const end_of_game&)
    {
        return l << "end_of_game{}";
    }
}

using event = std::variant
<
    events::next_input_creation,
    events::next_input_introduction,
    events::input_layout_change,
    events::input_introduction,
    events::item_drop,
    events::element_transmutation,
    events::element_unlocking,
    events::score_change,
    events::end_of_game
>;

using event_list = std::vector<event>;

} //namespace libgame

#endif
