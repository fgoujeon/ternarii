/*
Copyright 2018 Florian Goujeon

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

#include "tile.hpp"
#include <map>

namespace libview
{

namespace
{
    const auto label_vertical_margin_normalized = 0.2;
    const auto label_height_normalized = 1 - 2 * label_vertical_margin_normalized;

    SDL_Color get_background_color(const unsigned int value)
    {
        const auto value_color_map = std::map<unsigned int, SDL_Color>
        {
            {0,  SDL_Color{0x82, 0x3b, 0x3b, 0xff}},
            {1,  SDL_Color{0xe2, 0x1c, 0x1c, 0xff}},
            {2,  SDL_Color{0xe1, 0x82, 0x1d, 0xff}},
            {3,  SDL_Color{0xd2, 0xd8, 0x26, 0xff}},
            {4,  SDL_Color{0x7b, 0xb3, 0x4b, 0xff}},
            {5,  SDL_Color{0x4a, 0xb4, 0x50, 0xff}},
            {6,  SDL_Color{0x51, 0xad, 0x88, 0xff}},
            {7,  SDL_Color{0x51, 0xa3, 0xad, 0xff}},
            {8,  SDL_Color{0x50, 0x73, 0xae, 0xff}},
            {9,  SDL_Color{0x5c, 0x4e, 0xb0, 0xff}},
            {10, SDL_Color{0x8e, 0x51, 0xad, 0xff}},
            {11, SDL_Color{0xb4, 0x4a, 0xa2, 0xff}},
            {12, SDL_Color{0xe7, 0x00, 0xc1, 0xff}},
            {13, SDL_Color{0x98, 0x98, 0x98, 0xff}},
            {14, SDL_Color{0x46, 0x1b, 0x3f, 0xff}},
            {15, SDL_Color{0x00, 0x00, 0x00, 0xff}}
        };

        const auto it = value_color_map.find(value);
        if(it != value_color_map.end())
            return it->second;
        else
            return SDL_Color{0x00, 0x00, 0x00, 0xff};
    }

    point get_label_position
    (
        const point& tile_position,
        const unsigned int w,
        const unsigned int h
    )
    {
        return point
        {
            tile_position.x,
            tile_position.y + h * label_vertical_margin_normalized,
        };
    }
}

tile::tile
(
    SDL_Renderer& renderer,
    const unsigned int value,
    const point& position,
    const unsigned int w,
    const unsigned int h
):
    renderer_(renderer),
    position_(position),
    w_(w),
    h_(h),
    background_color_(get_background_color(value)),
    label_
    (
        renderer,
        get_label_position(position, w, h),
        w,
        label_height_normalized * h,
        std::to_string(value),
        horizontal_alignment::center,
        vertical_alignment::center,
        "res/fonts/DejaVuSans.ttf",
        SDL_Color{0xff, 0xff, 0xff, 0xff}
    )
{
}

const point& tile::get_position() const
{
    return position_;
}

void tile::set_position(const point& position)
{
    position_ = position;
    label_.set_position(get_label_position(position, w_, h_));
}

void tile::set_visible(const bool visible)
{
    visible_ = visible;
}

void tile::draw()
{
    if(!visible_)
        return;

    //draw background box
    {
        const auto& c = background_color_;
        const auto r = SDL_Rect
        {
            static_cast<int>(position_.x),
            static_cast<int>(position_.y),
            static_cast<int>(w_),
            static_cast<int>(h_)
        };

        SDL_SetRenderDrawColor(&renderer_, c.r, c.g, c.b, c.a);
        SDL_RenderFillRect(&renderer_, &r);
    }

    //draw number
    label_.draw();
}

} //namespace view
