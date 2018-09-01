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
