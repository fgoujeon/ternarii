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
#include "draw.hpp"
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
            {0,  SDL_Color{0xa0, 0x52, 0x52, 0xff}},
            {1,  SDL_Color{0xdd, 0x3b, 0x3b, 0xff}},
            {2,  SDL_Color{0xef, 0x74, 0x29, 0xff}},
            {3,  SDL_Color{0xe5, 0xbb, 0x13, 0xff}},
            {4,  SDL_Color{0xa1, 0xc9, 0x27, 0xff}},
            {5,  SDL_Color{0x2b, 0xcd, 0x73, 0xff}},
            {6,  SDL_Color{0x25, 0xd7, 0xd2, 0xff}},
            {7,  SDL_Color{0x3f, 0x94, 0xde, 0xff}},
            {8,  SDL_Color{0x4c, 0x52, 0xe0, 0xff}},
            {9,  SDL_Color{0x90, 0x4c, 0xe0, 0xff}},
            {10, SDL_Color{0xd8, 0x4c, 0xe0, 0xff}},
            {11, SDL_Color{0xe9, 0x98, 0xed, 0xff}},
            {12, SDL_Color{0x98, 0x98, 0x98, 0xff}},
            {13, SDL_Color{0x46, 0x1b, 0x3f, 0xff}}
        };

        const auto it = value_color_map.find(value);
        if(it != value_color_map.end())
            return it->second;
        else
            return SDL_Color{0x00, 0x00, 0x00, 0xff};
    }

    geometry::point get_label_position(const geometry::rect& tile_area)
    {
        return geometry::point
        {
            tile_area.pos.x,
            tile_area.pos.y + tile_area.h * label_vertical_margin_normalized,
        };
    }
}

tile::tile
(
    SDL_Renderer& renderer,
    const unsigned int value,
    const geometry::rect& area
):
    renderer_(renderer),
    area_(area),
    background_color_(get_background_color(value)),
    label_
    (
        renderer,
        "res/fonts/DejaVuSans.ttf",
        label_height_normalized * area.h,
        SDL_Color{0xff, 0xff, 0xff, 0xff},
        get_label_position(area),
        area.w,
        label_height_normalized * area.h,
        std::to_string(value),
        horizontal_alignment::center,
        vertical_alignment::center
    )
{
}

const geometry::point& tile::get_position() const
{
    return area_.pos;
}

void tile::set_position(const geometry::point& position)
{
    area_.pos = position;
    label_.set_position(get_label_position(area_));
}

void tile::set_visible(const bool visible)
{
    visible_ = visible;
}

void tile::draw(const geometry::system& sys)
{
    if(!visible_)
        return;

    //draw background box
    {
        const auto& c = background_color_;
        const auto r = SDL_Rect
        {
            static_cast<int>(area_.pos.x),
            static_cast<int>(area_.pos.y),
            static_cast<int>(area_.w),
            static_cast<int>(area_.h)
        };

        SDL_SetRenderDrawColor(&renderer_, c.r, c.g, c.b, c.a);
        draw_rect(renderer_, sys, r);
    }

    //draw number
    label_.draw(sys);
}

} //namespace view
