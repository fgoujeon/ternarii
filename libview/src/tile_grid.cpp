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

#include "tile_grid.hpp"
#include "tile.hpp"

namespace libview
{

namespace
{
    Magnum::Vector2 tile_coordinate_to_position(const data_types::tile_coordinate& c)
    {
        return
        {
            -2.5f + c.x,
            -5.0f + c.y
        };
    }

    std::array<Magnum::Vector2, 2> get_input_tile_positions(const unsigned int x_offset, const unsigned int rotation)
    {
        auto tile0_x = 0.0f;
        auto tile0_y = 0.0f;
        auto tile1_x = 0.0f;
        auto tile1_y = 0.0f;

        switch(rotation)
        {
            case 0:
                tile0_x = -0.5f;
                tile0_y = 0.0f;
                tile1_x = 0.5f;
                tile1_y = 0.0f;
                break;
            case 1:
                tile0_x = -0.5f;
                tile0_y = 0.5f;
                tile1_x = -0.5f;
                tile1_y = -0.5f;
                break;
            case 2:
                tile0_x = 0.5f;
                tile0_y = 0.0f;
                tile1_x = -0.5f;
                tile1_y = 0.0f;
                break;
            case 3:
                tile0_x = -0.5f;
                tile0_y = -0.5f;
                tile1_x = -0.5f;
                tile1_y = 0.5f;
                break;
        }

        return
        {
            Magnum::Vector2
            {
                -2.0f + tile0_x + x_offset,
                3.0f + tile0_y
            },
            Magnum::Vector2
            {
                -2.0f + tile1_x + x_offset,
                3.0f + tile1_y
            }
        };
    }
}

tile_grid::tile_grid(SceneGraph::DrawableGroup2D& drawables, Object2D* parent):
    Object2D{parent},
    drawables_(drawables)
{
}

bool tile_grid::is_animating() const
{
    return !animations_.empty();
}

void tile_grid::clear()
{
    for(const auto ptile: next_input_tiles_)
    {
        delete ptile;
    }
    next_input_tiles_.fill(nullptr);

    for(const auto ptile: input_tiles_)
    {
        delete ptile;
    }
    input_tiles_.fill(nullptr);

    for(auto& tiles: board_tiles_)
    {
        for(const auto ptile: tiles)
        {
            delete ptile;
        }
        tiles.fill(nullptr);
    }
}

void tile_grid::create_next_input(const unsigned int value0, const unsigned int value1)
{
    auto& animation = animations_.emplace_back();
    const auto animation_duration_s = 0.15f;

    next_input_tiles_[0] = &add_tile(value0, {-0.5f, 5.0f});
    animation.add_alpha_transition(0, 0.4, animation_duration_s, *next_input_tiles_[0]);

    next_input_tiles_[1] = &add_tile(value1, {0.5f, 5.0f});
    animation.add_alpha_transition(0, 0.4, animation_duration_s, *next_input_tiles_[1]);

    //simultaneously animate insertion of next input
    const auto dst_positions = get_input_tile_positions(input_x_offset_, input_rotation_);
    for(auto i = 0; i < 2; ++i)
    {
        if(input_tiles_[i] != nullptr)
        {
            animation.add_fixed_duration_translation
            (
                input_tiles_[i]->transformation().translation(),
                dst_positions[i],
                animation_duration_s,
                *input_tiles_[i]
            );
            animation.add_alpha_transition(0.4, 1, animation_duration_s, *input_tiles_[i]);
        }
    }
}

void tile_grid::insert_next_input(const unsigned int x_offset, const unsigned int rotation)
{
    input_x_offset_ = x_offset;
    input_rotation_ = rotation;
    std::swap(input_tiles_[0], next_input_tiles_[0]);
    std::swap(input_tiles_[1], next_input_tiles_[1]);
    //Note: Animation is done in create_next_input().
}

void tile_grid::set_input_layout(const unsigned int x_offset, const unsigned int rotation)
{
    if(input_x_offset_ != x_offset || input_rotation_ != rotation)
    {
        input_x_offset_ = x_offset;
        input_rotation_ = rotation;

        const auto dst_positions = get_input_tile_positions(input_x_offset_, input_rotation_);

        auto& animation = animations_.emplace_back();

        for(auto i = 0; i < 2; ++i)
        {
            animation.add_fixed_speed_translation
            (
                input_tiles_[i]->transformation().translation(),
                dst_positions[i],
                20,
                *input_tiles_[i]
            );
        }
    }
}

void tile_grid::insert_input
(
    const unsigned int tile0_dst_column_index,
    const unsigned int tile0_dst_row_index,
    const unsigned int tile1_dst_column_index,
    const unsigned int tile1_dst_row_index
)
{
    if(input_tiles_[0])
    {
        std::swap
        (
            board_tiles_[tile0_dst_column_index][tile0_dst_row_index],
            input_tiles_[0]
        );
    }

    if(input_tiles_[1])
    {
        std::swap
        (
            board_tiles_[tile1_dst_column_index][tile1_dst_row_index],
            input_tiles_[1]
        );
    }
}

void tile_grid::drop_tiles(const data_types::tile_drop_list& drops)
{
    auto& animation = animations_.emplace_back();

    for(const auto& drop: drops)
    {
        auto& ptile = board_tiles_[drop.column_index][drop.src_row_index];

        const auto src_position = tile_coordinate_to_position(data_types::tile_coordinate{drop.column_index, drop.src_row_index});
        const auto dst_position = tile_coordinate_to_position(data_types::tile_coordinate{drop.column_index, drop.dst_row_index});

        animation.add_fixed_speed_translation
        (
            src_position,
            dst_position,
            24,
            *ptile
        );

        std::swap(board_tiles_[drop.column_index][drop.dst_row_index], ptile);
    }
}

void tile_grid::merge_tiles(const data_types::tile_merge_list& merges)
{
    auto& animation0 = animations_.emplace_back();
    auto& animation1 = animations_.emplace_back();
    std::vector<tile*> tiles_to_delete;

    for(const auto& merge: merges)
    {
        const auto dst_position = tile_coordinate_to_position(merge.dst_tile_coordinate);

        for(const auto& src_tile_coordinate: merge.src_tile_coordinates) //for each source tile
        {
            const auto src_position = tile_coordinate_to_position(src_tile_coordinate);
            auto& src_tile = *board_tiles_[src_tile_coordinate.x][src_tile_coordinate.y];

            //first, translate source tile toward position of destination tile
            if(dst_position != src_position)
            {
                animation0.add_fixed_speed_translation
                (
                    src_position,
                    dst_position,
                    6,
                    src_tile
                );
            }

            //then, make it disappear with a fade out
            animation1.add_alpha_transition(1, 0, 0.2, src_tile);

            tiles_to_delete.push_back(&src_tile);
            board_tiles_[src_tile_coordinate.x][src_tile_coordinate.y] = nullptr;
        }

        //create destination tile
        auto& dst_tile = add_tile(merge.dst_tile_value, dst_position);
        board_tiles_[merge.dst_tile_coordinate.x][merge.dst_tile_coordinate.y] = &dst_tile;

        //make destination tile appear with a fade in
        animation1.add_alpha_transition(0, 1, 0.2, dst_tile);
    }

    animation1.set_cleanup_callback
    (
        [this, tiles_to_delete]()
        {
            for(auto ptile: tiles_to_delete)
            {
                drawables_.remove(*ptile);
                delete ptile;
            }
        }
    );
}

void tile_grid::advance()
{
    if(!animations_.empty())
    {
        auto& animation = animations_.front();
        if(!animation.is_done())
        {
            animation.advance();
        }
        else
        {
            animations_.pop_front();
        }
    }
}

tile& tile_grid::add_tile
(
    const int value,
    const Magnum::Vector2& position
)
{
    auto& t = addChild<tile>(value, drawables_);
    t.scale({0.45f, 0.45f});
    t.translate(position);
    return t;
}

} //namespace
