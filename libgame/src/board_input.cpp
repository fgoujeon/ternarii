#include "board_input.hpp"

namespace libgame
{

namespace
{
    const auto default_x_offset = 2;
    const auto default_rotation = 0;
}

board_input::board_input(const tile_pair& tiles):
    tiles_(tiles),
    x_offset_(default_x_offset),
    rotation_(default_rotation)
{
}

event
board_input::set_tiles(const tile_pair& tiles)
{
    tiles_ = tiles;
    x_offset_ = default_x_offset;
    rotation_ = default_rotation;
    apply();

    return
    {
        events::next_input_insertion
        {
            x_offset_,
            rotation_
        }
    };
}

std::vector<event>
board_input::shift_left()
{
    if(x_offset_ > 0)
    {
        --x_offset_;
        return apply();
    }

    return std::vector<event>();
}

std::vector<event>
board_input::shift_right()
{
    if(x_offset_ < column_count - 1)
    {
        ++x_offset_;
        return apply();
    }

    return std::vector<event>();
}

std::vector<event>
board_input::rotate()
{
    rotation_ = (rotation_ + 1) % 4;
    return apply();
}

std::vector<event>
board_input::apply()
{
    //adjust the offset so that the tiles stay inside the board
    if(x_offset_ >= column_count - 1 && (rotation_ == 0 || rotation_ == 2))
        x_offset_ = column_count - 2;

    return
    {
        events::input_layout_change{x_offset_, rotation_}
    };
}

} //namespace libgame
