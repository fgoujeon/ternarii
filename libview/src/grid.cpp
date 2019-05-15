/*
Copyright 2018 - 2019 Florian Goujeon

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
#include "draw.hpp"
#include "utility.hpp"
#include <map>

namespace libview
{

namespace
{
    const auto cell_size = 100;
    const auto column_count = 6;
    const auto row_count = 11;
    const auto tile_margin = cell_size * 0.05;
    const auto tile_size = cell_size - tile_margin * 2;

    //speeds, in pixels per second
    const auto tile_insertion_speed = 1200;
    const auto tile_shift_speed = 1200;
    const auto tile_rotation_speed = 1200;
    const auto tile_drop_speed = 2400;
    const auto tile_merge_speed = 600;

    auto tile_coordinate_to_position(const data_types::tile_coordinate& c)
    {
        return geometry::point
        {
            c.x * cell_size + tile_margin,
            (10 - c.y) * cell_size + tile_margin
        };
    }

    std::array<geometry::point, 2> get_input_tile_positions(const unsigned int x_offset, const unsigned int rotation)
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

        return std::array<geometry::point, 2>
        {
            geometry::point
            {
                (tile0_x + x_offset) * cell_size + tile_margin,
                (tile0_y + 1.5) * cell_size + tile_margin
            },
                geometry::point
            {
                (tile1_x + x_offset) * cell_size + tile_margin,
                (tile1_y + 1.5) * cell_size + tile_margin
            }
        };
    }
}

grid::grid(SDL_Renderer& renderer):
    renderer_(renderer)
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

void grid::clear()
{
    for(auto& ptile: next_input_tiles_)
        ptile.reset();

    for(auto& ptile: input_tiles_)
        ptile.reset();

    for(auto& tile_column: board_tiles_)
        for(auto& ptile: tile_column)
            ptile.reset();

    disappearing_tiles_.clear();

    libview::clear(animations_);
}

void grid::create_next_input(const unsigned int value0, const unsigned int value1)
{
    const auto values = std::array<unsigned int, 2>{value0, value1};

    auto i = 0;
    for(const auto value: values)
    {
        next_input_tiles_[i] = std::make_unique<tile>
        (
            renderer_,
            value,
            geometry::rect
            {
                (2 + i) * cell_size + tile_margin,
                tile_margin,
                tile_size,
                tile_size
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

    auto pgroup = std::make_unique<animation_group>();
    for(auto i = 0; i < 2; ++i)
    {
        input_tiles_[i] = std::move(next_input_tiles_[i]);
        pgroup->emplace<translation>(*input_tiles_[i], dst_positions[i], tile_insertion_speed);
    }

    animations_.push(std::move(pgroup));
}

void grid::set_input_x_offset(const unsigned int value)
{
    if(input_x_offset_ != value)
    {
        input_x_offset_ = value;

        const auto dst_positions = get_input_tile_positions(input_x_offset_, input_rotation_);

        auto pgroup = std::make_unique<animation_group>();
        for(auto i = 0; i < 2; ++i)
            pgroup->emplace<translation>(*input_tiles_[i], dst_positions[i], tile_shift_speed);

        animations_.push(std::move(pgroup));
    }
}

void grid::set_input_rotation(const unsigned int value)
{
    if(input_rotation_ != value)
    {
        input_rotation_ = value;

        const auto dst_positions = get_input_tile_positions(input_x_offset_, input_rotation_);

        auto pgroup = std::make_unique<animation_group>();
        for(auto i = 0; i < 2; ++i)
            pgroup->emplace<translation>(*input_tiles_[i], dst_positions[i], tile_rotation_speed);

        animations_.push(std::move(pgroup));
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
    auto pgroup = std::make_unique<animation_group>();

    for(const auto& drop: drops)
    {
        if(auto& ptile = board_tiles_[drop.column_index][drop.src_row_index])
        {
            const auto dst_position = tile_coordinate_to_position(data_types::tile_coordinate{drop.column_index, drop.dst_row_index});
            pgroup->emplace<translation>(*ptile, dst_position, tile_drop_speed);
            board_tiles_[drop.column_index][drop.dst_row_index] = std::move(ptile);
        }
    }

    animations_.push(std::move(pgroup));
    animations_.push(std::make_unique<pause>(0.05));
}

void grid::merge_tiles(const data_types::tile_merge_list& merges)
{
    //translate source tiles to position of destination tile
    {
        auto pgroup = std::make_unique<animation_group>();

        for(const auto& merge: merges)
        {
            const auto dst_position = tile_coordinate_to_position(merge.dst_tile_coordinate);

            for(const auto& src_tile_coordinate: merge.src_tile_coordinates)
            {
                if(auto& ptile = board_tiles_[src_tile_coordinate.x][src_tile_coordinate.y])
                {
                    pgroup->emplace<translation>(*ptile, dst_position, tile_merge_speed);
                }
            }
        }

        animations_.push(std::move(pgroup));
    }

    //make source tiles disappear and create merged tile
    {
        auto pgroup = std::make_unique<animation_group>();

        for(const auto& merge: merges)
        {
            //make source tiles disappear
            for(const auto& src_tile_coordinate: merge.src_tile_coordinates)
            {
                if(auto& ptile = board_tiles_[src_tile_coordinate.x][src_tile_coordinate.y])
                {
                    pgroup->emplace<fade_out>(*ptile);
                    disappearing_tiles_.push_back(std::move(ptile));
                }
            }

            //create merged tile
            auto pdst_tile = std::make_unique<tile>
            (
                renderer_,
                merge.dst_tile_value,
                geometry::rect
                {
                    tile_coordinate_to_position(merge.dst_tile_coordinate),
                    tile_size,
                    tile_size
                }
            );
            pgroup->add(std::make_unique<fade_in>(*pdst_tile));

            board_tiles_[merge.dst_tile_coordinate.x][merge.dst_tile_coordinate.y] = std::move(pdst_tile);
        }

        animations_.push(std::move(pgroup));
    }

    //make a pause
    animations_.push(std::make_unique<pause>(0.2));
}

void grid::draw(SDL_Renderer& renderer, const geometry::system& sys, const double ellapsed_time)
{
    //animate
    iterate(animations_, ellapsed_time);
    if(animations_.empty())
        disappearing_tiles_.clear();

    //draw background
    {
        const auto r = SDL_Rect{0, 0, column_count * cell_size, row_count * cell_size};
        SDL_SetRenderDrawColor(&renderer, 0x66, 0x66, 0x66, 0xff);
        draw_rect(renderer, sys, r);
    }

    //draw tiles
    {
        for(auto& ptile: next_input_tiles_)
            if(ptile)
                ptile->draw(sys);

        for(auto& ptile: input_tiles_)
            if(ptile)
                ptile->draw(sys);

        for(auto& tile_column: board_tiles_)
            for(auto& ptile: tile_column)
                if(ptile)
                    ptile->draw(sys);

        for(auto& ptile: disappearing_tiles_)
            if(ptile)
                ptile->draw(sys);
    }

    //draw next input line
    {
        SDL_SetRenderDrawColor(&renderer, 0xff, 0xff, 0xff, 0x7f);
        for(auto x = 0; x < column_count * cell_size; x += 16)
        {
            const auto r = SDL_Rect{x, cell_size - 2, 8, 4};
            draw_rect(renderer, sys, r);
        }
    }

    //draw death line
    {
        SDL_SetRenderDrawColor(&renderer, 0xff, 0xff, 0xff, 0x7f);
        for(auto x = 0; x < column_count * cell_size; x += 16)
        {
            const auto r = SDL_Rect{x, 4 * cell_size - 2, 8, 4};
            draw_rect(renderer, sys, r);
        }
    }

    //draw semi-transparent foreground over next input
    {
        const auto r = SDL_Rect{0, 0, column_count * cell_size, 1 * cell_size};
        SDL_SetRenderDrawColor(&renderer, 0x66, 0x66, 0x66, 0x7f);
        draw_rect(renderer, sys, r);
    }
}

bool grid::is_animating() const
{
    return !animations_.empty();
}

} //namespace view
