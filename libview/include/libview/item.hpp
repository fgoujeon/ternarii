#ifndef LIBVIEW_ITEM_HPP
#define LIBVIEW_ITEM_HPP

#include <functional>
#include <optional>
#include <array>

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

using next_input_item_array = item_array<6, 2>;
using input_item_array = item_array<6, 2>;
using board_item_array = item_array<6, 10>;

} //namespace view

#endif
