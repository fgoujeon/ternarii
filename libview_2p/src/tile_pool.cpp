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

#include "tile_pool.hpp"
#include "draw.hpp"
#include "utility.hpp"
#include <map>

namespace libview_2p
{

namespace
{
    const auto cell_size = 100;
    const auto column_count = 6;
    const auto row_count = 2;
    const auto tile_margin = cell_size * 0.05;
    const auto tile_size = cell_size - tile_margin * 2;

    auto tile_coordinate_to_position(const data_types::tile_coordinate& c)
    {
        return geometry::point
        {
            c.x * cell_size + tile_margin,
            (10 - c.y) * cell_size + tile_margin
        };
    }
}

tile_pool::tile_pool(SDL_Renderer& renderer):
    renderer_(renderer)
{
}

int tile_pool::get_logical_width() const
{
    return column_count * cell_size;
}

int tile_pool::get_logical_height() const
{
    return row_count * cell_size;
}

void tile_pool::clear()
{
    for(auto& tile_column: tiles_)
        for(auto& ptile: tile_column)
            ptile.reset();
}

void tile_pool::set_tiles(const data_types::tile_pool& tiles)
{
    for(unsigned int column_index = 0; column_index < 6; ++column_index)
    {
        for(unsigned int row_index = 0; row_index < 2; ++row_index)
        {
            tiles_[column_index][row_index] = std::make_unique<tile>
            (
                renderer_,
                tiles[column_index][row_index].value,
                geometry::rect
                {
                    column_index * cell_size + tile_margin,
                    row_index * cell_size + tile_margin,
                    tile_size,
                    tile_size
                }
            );
            tiles_[column_index][row_index]->set_visible(true);
        }
    }
}

void tile_pool::draw(SDL_Renderer& renderer, const geometry::system& sys)
{
    //draw background
    {
        const auto r = SDL_Rect{0, 0, column_count * cell_size, row_count * cell_size};
        SDL_SetRenderDrawColor(&renderer, 0x66, 0x66, 0x66, 0xff);
        draw_rect(renderer, sys, r);
    }

    //draw tiles
    {
        for(auto& tile_column: tiles_)
            for(auto& ptile: tile_column)
                if(ptile)
                    ptile->draw(sys);
    }
}

} //namespace view
