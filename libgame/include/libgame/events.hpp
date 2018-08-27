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
    struct next_input_creation
    {
        std::array<tile, 2> tiles;
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
        unsigned int column_index;
        unsigned int src_row_index;
        unsigned int dst_row_index;
    };

    inline
    std::ostream& operator<<(std::ostream& l, const tile_drop& r)
    {
        l << "tile_drop";
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
    events::next_input_insertion,
    events::input_layout_change,
    events::input_insertion,
    events::tile_drop,
    events::element_transmutation,
    events::element_unlocking,
    events::score_change,
    events::end_of_game
>;

using event_list = std::vector<event>;

} //namespace libgame

#endif
