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

namespace libview
{

namespace
{
    const auto cell_size = 50;
    const auto column_count = 6;
    const auto row_count = 12;
    const auto tile_margin = cell_size * 0.05;
    const auto tile_size = cell_size - tile_margin * 2;

    //speeds, in pixels per second
    const auto tile_insertion_speed = 600;
    const auto tile_shift_speed = 600;
    const auto tile_rotation_speed = 600;
    const auto tile_drop_speed = 1200;
    const auto tile_merge_speed = 300;

    auto tile_coordinate_to_position(const data_types::tile_coordinate& c)
    {
        return point
        {
            c.x * cell_size + tile_margin,
            (11 - c.y) * cell_size + tile_margin
        };
    }

    std::array<point, 2> get_input_tile_positions(const unsigned int x_offset, const unsigned int rotation)
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

        return std::array<point, 2>
        {
            point
            {
                (tile0_x + x_offset) * cell_size + tile_margin,
                (tile0_y + 2) * cell_size + tile_margin
            },
            point
            {
                (tile1_x + x_offset) * cell_size + tile_margin,
                (tile1_y + 2) * cell_size + tile_margin
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
            point
            {
                (2 + i) * cell_size + tile_margin,
                tile_margin
            }
        );
        next_input_tiles_[i]->set_visible(true);
        ++i;
    }
}

void grid::insert_next_input(const unsigned int x_offset, const unsigned int rotation)
{
    input_x_offset_ = x_offset;
    input_rotation_ = rotation;

    const auto dst_positions = get_input_tile_positions(x_offset, rotation);

    animation_group g;
    for(auto i = 0; i < 2; ++i)
    {
        input_tiles_[i] = std::move(next_input_tiles_[i]);
        g.push_back(std::make_unique<translation>(*input_tiles_[i], dst_positions[i], tile_insertion_speed));
    }

    animations_.push(std::move(g));
}

void grid::set_next_input_items(const data_types::next_input_item_array& items)
{
//    auto i = 0;
//    for(auto& opt_item: items)
//    {
//        if(opt_item)
//        {
//            next_input_tiles_[i] = std::make_unique<tile>();
//            next_input_tiles_[i]->set_value(opt_item->value);
//            next_input_tiles_[i]->set_position
//            (
//                SDL_Point
//                {
//                    static_cast<int>((2 + i) * cell_size + tile_margin),
//                    static_cast<int>(tile_margin)
//                }
//            );
//        }
//        else
//        {
//            next_input_tiles_[i] = nullptr;
//        }
//
//        ++i;
//    }
}

void grid::set_input_items(const data_types::input_item_array& items)
{
//    auto i = 0;
//    for(auto& opt_item: items)
//    {
//        if(opt_item)
//        {
//            input_tiles_[i] = std::make_unique<tile>();
//            input_tiles_[i]->set_value(opt_item->value);
//        }
//        else
//        {
//            input_tiles_[i] = nullptr;
//        }
//
//        ++i;
//    }
//
//    update_input_tile_areas();
}

void grid::set_input_x_offset(const unsigned int value)
{
    if(input_x_offset_ != value)
    {
        input_x_offset_ = value;

        const auto dst_positions = get_input_tile_positions(input_x_offset_, input_rotation_);

        animation_group g;
        for(auto i = 0; i < 2; ++i)
            g.push_back(std::make_unique<translation>(*input_tiles_[i], dst_positions[i], tile_shift_speed));

        animations_.push(std::move(g));
    }
}

void grid::set_input_rotation(const unsigned int value)
{
    if(input_rotation_ != value)
    {
        input_rotation_ = value;

        const auto dst_positions = get_input_tile_positions(input_x_offset_, input_rotation_);

        animation_group g;
        for(auto i = 0; i < 2; ++i)
            g.push_back(std::make_unique<translation>(*input_tiles_[i], dst_positions[i], tile_rotation_speed));

        animations_.push(std::move(g));
    }
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

void grid::drop_tiles(const data_types::tile_drop_list& drops)
{
    animation_group g;

    for(const auto& drop: drops)
    {
        if(auto& ptile = board_tiles_[drop.column_index][drop.src_row_index])
        {
            const auto dst_position = tile_coordinate_to_position(data_types::tile_coordinate{drop.column_index, drop.dst_row_index});
            g.push_back(std::make_unique<translation>(*ptile, dst_position, tile_drop_speed));
            board_tiles_[drop.column_index][drop.dst_row_index] = std::move(ptile);
        }
    }

    animations_.push(std::move(g));

    animation_group pause_group;
    pause_group.push_back(std::make_unique<pause>(0.1));
    animations_.push(std::move(pause_group));
}

void grid::merge_tiles(const data_types::tile_merge_list& merges)
{
    //translate source tiles to position of destination tile
    {
        animation_group g;

        for(const auto& merge: merges)
        {
            const auto dst_position = tile_coordinate_to_position(merge.dst_tile_coordinate);

            for(const auto& src_tile_coordinate: merge.src_tile_coordinates)
            {
                if(auto& ptile = board_tiles_[src_tile_coordinate.x][src_tile_coordinate.y])
                {
                    g.push_back(std::make_unique<translation>(*ptile, dst_position, tile_merge_speed));
                }
            }
        }

        animations_.push(std::move(g));
    }

    //make source tiles disappear
    {
        animation_group g;

        for(const auto& merge: merges)
        {
            for(const auto& src_tile_coordinate: merge.src_tile_coordinates)
            {
                if(auto& ptile = board_tiles_[src_tile_coordinate.x][src_tile_coordinate.y])
                {
                    g.push_back(std::make_unique<fade_out>(*ptile));
                }
            }
        }

        animations_.push(std::move(g));
    }

    for(const auto& merge: merges)
    {
        for(const auto& src_tile_coordinate: merge.src_tile_coordinates)
        {
            if(auto& ptile = board_tiles_[src_tile_coordinate.x][src_tile_coordinate.y])
            {
                disappearing_tiles_.push_back(std::move(ptile));
            }
        }
    }

    //create merged tile
    for(const auto& merge: merges)
    {
        auto pdst_tile = std::make_unique<tile>();
        pdst_tile->set_value(merge.dst_tile_value);
        pdst_tile->set_size(tile_size, tile_size);
        pdst_tile->set_position(tile_coordinate_to_position(merge.dst_tile_coordinate));

        animation_group g;
        g.push_back(std::make_unique<fade_in>(*pdst_tile));
        animations_.push(std::move(g));

        board_tiles_[merge.dst_tile_coordinate.x][merge.dst_tile_coordinate.y] = std::move(pdst_tile);
    }

    //make a pause
    {
        animation_group pause_group;
        pause_group.push_back(std::make_unique<pause>(0.2));
        animations_.push(std::move(pause_group));
    }
}

void grid::set_board_items(const data_types::board_item_array& items)
{
//    fill_tiles(board_tiles_, items, 11);
}

void grid::draw(SDL_Renderer& renderer, const double ellapsed_time)
{
    //animate
    iterate(animations_, ellapsed_time);
    if(animations_.empty())
        disappearing_tiles_.clear();

    //draw background
    {
        const auto r = SDL_Rect{0, 0, 6 * cell_size, 12 * cell_size};
        SDL_SetRenderDrawColor(&renderer, 0x66, 0x66, 0x66, 255);
        SDL_RenderFillRect(&renderer, &r);
    }

    //draw death line
    {
        const auto r = SDL_Rect{0, 5 * cell_size - 1, 6 * cell_size, 2};
        SDL_SetRenderDrawColor(&renderer, 0xff, 0xff, 0xff, 255);
        SDL_RenderFillRect(&renderer, &r);
    }

    //draw tiles
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

        for(auto& ptile: disappearing_tiles_)
            if(ptile)
                ptile->draw(renderer);
    }
}

bool grid::is_animating() const
{
    return !animations_.empty();
}

} //namespace view
