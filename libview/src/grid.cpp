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

#include "grid.hpp"
#include "utility.hpp"
#include <map>
#include <iostream>

namespace libview
{

namespace
{
    const auto cell_size = 50;
    const auto column_count = 6;
    const auto row_count = 12;
    const auto tile_margin = cell_size * 0.05;
    const auto tile_size = cell_size - tile_margin * 2;

    auto tile_coordinate_to_position(const tile_coordinate& c)
    {
        return SDL_Point
        {
            static_cast<int>(c.x * cell_size + tile_margin),
            static_cast<int>((11 - c.y) * cell_size + tile_margin)
        };
    }

    template<class TileArray, class ItemArray>
    void fill_tiles(TileArray& tiles, const ItemArray& items, const unsigned int y_offset)
    {
        auto x = 0;
        for(auto& item_column: items)
        {
            auto y = 0;
            for(auto& opt_item: item_column)
            {
                if(opt_item)
                {
                    auto ptile = std::make_unique<tile>();
                    ptile->set_value(opt_item->value);
                    ptile->set_position
                    (
                        SDL_Point
                        {
                            static_cast<int>(x * cell_size + tile_margin),
                            static_cast<int>((y_offset - y) * cell_size + tile_margin)
                        }
                    );
                    tiles[x][y] = std::move(ptile);
                }
                else
                {
                    tiles[x][y] = nullptr;
                }

                ++y;
            }

            ++x;
        }
    }

    std::array<SDL_Point, 2> get_input_tile_positions(const unsigned int x_offset, const unsigned int rotation)
    {
        auto tile0_x = 0.0;
        auto tile0_y = 0.0;
        auto tile1_x = 0.0;
        auto tile1_y = 0.0;

        switch(rotation)
        {
            case 0:
                tile0_x = 0;
                tile0_y = 0.5;
                tile1_x = 1;
                tile1_y = 0.5;
                break;
            case 1:
                tile0_x = 0;
                tile0_y = 0;
                tile1_x = 0;
                tile1_y = 1;
                break;
            case 2:
                tile0_x = 1;
                tile0_y = 0.5;
                tile1_x = 0;
                tile1_y = 0.5;
                break;
            case 3:
                tile0_x = 0;
                tile0_y = 1;
                tile1_x = 0;
                tile1_y = 0;
                break;
        }

        return std::array<SDL_Point, 2>
        {
            SDL_Point
            {
                static_cast<int>((tile0_x + x_offset) * cell_size + tile_margin),
                static_cast<int>((tile0_y + 2) * cell_size + tile_margin)
            },
            SDL_Point
            {
                static_cast<int>((tile1_x + x_offset) * cell_size + tile_margin),
                static_cast<int>((tile1_y + 2) * cell_size + tile_margin)
            }
        };
    }
}

grid::grid(SDL_Renderer& renderer)
{
}

int grid::get_logical_width() const
{
    return column_count * cell_size;
}

int grid::get_logical_height() const
{
    return row_count * cell_size;
}

void grid::create_next_input(const unsigned int value0, const unsigned int value1)
{
    const auto values = std::array<unsigned int, 2>{value0, value1};

    auto i = 0;
    for(const auto value: values)
    {
        next_input_tiles_[i] = std::make_unique<tile>();
        next_input_tiles_[i]->set_value(value);
        next_input_tiles_[i]->set_size(tile_size, tile_size);
        next_input_tiles_[i]->set_position
        (
            SDL_Point
            {
                static_cast<int>((2 + i) * cell_size + tile_margin),
                static_cast<int>(tile_margin)
            }
        );
        ++i;
    }
}

void grid::insert_next_input(const unsigned int x_offset, const unsigned int rotation)
{
    for(auto i = 0; i < 2; ++i)
        input_tiles_[i] = std::move(next_input_tiles_[i]);

    input_x_offset_ = x_offset;
    input_rotation_ = rotation;
    update_input_tile_areas();
}

void grid::set_next_input_items(const next_input_item_array& items)
{
    auto i = 0;
    for(auto& opt_item: items)
    {
        if(opt_item)
        {
            next_input_tiles_[i] = std::make_unique<tile>();
            next_input_tiles_[i]->set_value(opt_item->value);
            next_input_tiles_[i]->set_position
            (
                SDL_Point
                {
                    static_cast<int>((2 + i) * cell_size + tile_margin),
                    static_cast<int>(tile_margin)
                }
            );
        }
        else
        {
            next_input_tiles_[i] = nullptr;
        }

        ++i;
    }
}

void grid::set_input_items(const input_item_array& items)
{
    auto i = 0;
    for(auto& opt_item: items)
    {
        if(opt_item)
        {
            input_tiles_[i] = std::make_unique<tile>();
            input_tiles_[i]->set_value(opt_item->value);
        }
        else
        {
            input_tiles_[i] = nullptr;
        }

        ++i;
    }

    update_input_tile_areas();
}

void grid::set_input_x_offset(const unsigned int value)
{
    input_x_offset_ = value;
    update_input_tile_areas();
}

void grid::set_input_rotation(const unsigned int value)
{
    input_rotation_ = value;
    update_input_tile_areas();
}

void grid::insert_input
(
    const unsigned int tile0_dst_column_index,
    const unsigned int tile0_dst_row_index,
    const unsigned int tile1_dst_column_index,
    const unsigned int tile1_dst_row_index
)
{
    if(input_tiles_[0])
        board_tiles_[tile0_dst_column_index][tile0_dst_row_index] = std::move(input_tiles_[0]);

    if(input_tiles_[1])
        board_tiles_[tile1_dst_column_index][tile1_dst_row_index] = std::move(input_tiles_[1]);
}

void grid::drop_tile
(
    const unsigned int column_index,
    const unsigned int src_row_index,
    const unsigned int dst_row_index
)
{
    auto& ptile = board_tiles_[column_index][src_row_index];
    if(ptile)
    {
        ptile->set_position(tile_coordinate_to_position(tile_coordinate{column_index, dst_row_index}));
        board_tiles_[column_index][dst_row_index] = std::move(ptile);
    }
}

void grid::merge_tiles
(
    const std::vector<tile_coordinate>& src_tiles,
    const tile_coordinate& dst_tile,
    const unsigned int dst_tile_value
)
{
    for(const auto& src_tile: src_tiles)
        board_tiles_[src_tile.x][src_tile.y] = nullptr;

    auto pdst_tile = std::make_unique<tile>();
    pdst_tile->set_value(dst_tile_value);
    pdst_tile->set_size(tile_size, tile_size);
    pdst_tile->set_position(tile_coordinate_to_position(dst_tile));
    board_tiles_[dst_tile.x][dst_tile.y] = std::move(pdst_tile);
}

void grid::set_board_items(const board_item_array& items)
{
    fill_tiles(board_tiles_, items, 11);
}

void grid::draw(SDL_Renderer& renderer)
{
    //background
    {
        const auto r = SDL_Rect{0, 0, 6 * cell_size, 12 * cell_size};
        SDL_SetRenderDrawColor(&renderer, 0x66, 0x66, 0x66, 255);
        SDL_RenderFillRect(&renderer, &r);
    }

    //death line
    {
        const auto r = SDL_Rect{0, 5 * cell_size - 1, 6 * cell_size, 2};
        SDL_SetRenderDrawColor(&renderer, 0xff, 0xff, 0xff, 255);
        SDL_RenderFillRect(&renderer, &r);
    }

    //tiles
    {
        for(auto& ptile: next_input_tiles_)
            if(ptile)
                ptile->draw(renderer);

        for(auto& ptile: input_tiles_)
            if(ptile)
                ptile->draw(renderer);

        for(auto& tile_column: board_tiles_)
            for(auto& ptile: tile_column)
                if(ptile)
                    ptile->draw(renderer);
    }
}

void grid::update_input_tile_areas()
{
    const auto positions = get_input_tile_positions(input_x_offset_, input_rotation_);
    for(auto i = 0; i < 2; ++i)
        if(auto& ptile = input_tiles_[i])
            ptile->set_position(positions[i]);
}

} //namespace view
