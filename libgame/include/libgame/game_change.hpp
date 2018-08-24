#ifndef GAME_CHANGE_HPP
#define GAME_CHANGE_HPP

#include <libgame/element.hpp>
#include <variant>
#include <vector>
#include <memory>
#include <array>

namespace libgame
{

namespace game_changes
{
    struct next_input_creation
    {
        std::array<std::shared_ptr<element>, 2> items;
    };

    struct next_input_introduction
    {
        std::array<std::shared_ptr<element>, 2> items;
        unsigned int x_offset;
        unsigned int rotation;
    };
}

namespace board_input_changes
{
    struct layout
    {
        unsigned int x_offset;
        unsigned int rotation;
    };

    struct clear{};
}

namespace board_changes
{
    struct item_drop
    {
        std::shared_ptr<element> elt;
        unsigned int column_index;
        unsigned int row_index;
    };

    struct element_transmutation
    {
        std::vector<std::shared_ptr<element>> source_elements;
        std::shared_ptr<element> destination_element;
        unsigned int column_index;
        unsigned int row_index;
    };
}

namespace game_changes
{
    struct element_unlocking
    {
        unsigned int index;
    };

    struct score_change
    {
        unsigned int score;
    };
}

using game_change_t = std::variant
<
    game_changes::next_input_creation,
    game_changes::next_input_introduction,
    board_input_changes::layout,
    board_input_changes::clear,
    board_changes::item_drop,
    board_changes::element_transmutation,
    game_changes::element_unlocking,
    game_changes::score_change
>;

using game_change_list = std::vector<game_change_t>;

} //namespace libgame

#endif
