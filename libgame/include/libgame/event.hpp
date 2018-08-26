#ifndef LIBGAME_EVENT_HPP
#define LIBGAME_EVENT_HPP

#include <libgame/element.hpp>
#include <variant>
#include <vector>
#include <memory>
#include <array>

namespace libgame
{

namespace events
{
    struct next_input_creation
    {
        std::array<element, 2> items;
    };

    struct next_input_introduction
    {
        unsigned int x_offset;
        unsigned int rotation;
    };

    struct input_layout_change
    {
        unsigned int x_offset;
        unsigned int rotation;
    };

    struct input_introduction
    {
        unsigned int tile0_dst_column_index;
        unsigned int tile0_dst_row_index;
        unsigned int tile1_dst_column_index;
        unsigned int tile1_dst_row_index;
    };

    struct item_drop
    {
        unsigned int column_index;
        unsigned int src_row_index;
        unsigned int dst_row_index;
    };

    struct element_transmutation
    {
        std::vector<std::shared_ptr<element>> source_elements;
        std::shared_ptr<element> destination_element;
        unsigned int column_index;
        unsigned int row_index;
    };

    struct element_unlocking
    {
        unsigned int index;
    };

    struct score_change
    {
        unsigned int score;
    };
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
    events::score_change
>;

using event_list = std::vector<event>;

} //namespace libgame

#endif
