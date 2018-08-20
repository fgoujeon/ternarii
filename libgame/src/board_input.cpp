#include "board_input.hpp"

namespace libgame
{

std::vector<game_change_t>
board_input::set_items(const board_next_input_t& items)
{
    items_ = items;
    x_offset_ = 2;
    rotation_ = 0;
    apply();

    return
    {
        game_changes::next_input_introduction
        {
            items,
            x_offset_,
            rotation_
        }
    };
}

std::vector<game_change_t>
board_input::shift_left()
{
    if(x_offset_ > 0)
    {
        --x_offset_;
        return apply();
    }

    return std::vector<game_change_t>();
}

std::vector<game_change_t>
board_input::shift_right()
{
    if(x_offset_ < column_count - 1)
    {
        ++x_offset_;
        return apply();
    }

    return std::vector<game_change_t>();
}

std::vector<game_change_t>
board_input::rotate()
{
    rotation_ = (rotation_ + 1) % 4;
    return apply();
}

std::vector<game_change_t>
board_input::apply()
{
    //adjust the offset so that the items are inside the board
    if(x_offset_ >= column_count - 1 && (rotation_ == 0 || rotation_ == 2))
        x_offset_ = column_count - 2;

    const unsigned int x0 = x_offset_ + (rotation_ == 2 ? 1 : 0);
    const unsigned int y0 = (rotation_ == 1 ? 1 : 0);

    const unsigned int x1 = x_offset_ + (rotation_ == 0 ? 1 : 0);
    const unsigned int y1 = (rotation_ == 3 ? 1 : 0);

    item_grid_.fill(std::nullopt);
    item_grid_.at(x0, y0) = items_[0];
    item_grid_.at(x1, y1) = items_[1];

    return
    {
        board_input_changes::layout{x_offset_, rotation_}
    };
}

} //namespace libgame
