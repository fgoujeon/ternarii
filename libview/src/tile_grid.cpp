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
    SceneGraph::Drawable2D{*this, &drawables},
    drawables_(drawables)
{
}

bool tile_grid::is_animating() const
{
    return !animations_.empty();
}

void tile_grid::create_next_input(const unsigned int value0, const unsigned int value1)
{
    next_input_tiles_[0] = &addChild<tile>(drawables_);
    next_input_tiles_[0]->translate({-0.5f, 5.0f});
    next_input_tiles_[0]->set_value(value0);
    next_input_tiles_[0]->set_alpha(0.5f);

    next_input_tiles_[1] = &addChild<tile>(drawables_);
    next_input_tiles_[1]->translate({0.5f, 5.0f});
    next_input_tiles_[1]->set_value(value1);
    next_input_tiles_[1]->set_alpha(0.5f);
}

void tile_grid::insert_next_input(const unsigned int x_offset, const unsigned int rotation)
{
    input_x_offset_ = x_offset;
    input_rotation_ = rotation;

    input_tiles_[0] = next_input_tiles_[0];
    input_tiles_[0]->set_alpha(1);

    input_tiles_[1] = next_input_tiles_[1];
    input_tiles_[1]->set_alpha(1);

    update_input_tiles_positions();
}

void tile_grid::set_input_x_offset(const unsigned int value)
{
    if(input_x_offset_ != value)
    {
        input_x_offset_ = value;
        update_input_tiles_positions();
    }
}

void tile_grid::set_input_rotation(const unsigned int value)
{
    if(input_rotation_ != value)
    {
        input_rotation_ = value;
        update_input_tiles_positions();
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
        board_tiles_[tile0_dst_column_index][tile0_dst_row_index] = input_tiles_[0];
        input_tiles_[0] = nullptr;
    }

    if(input_tiles_[1])
    {
        board_tiles_[tile1_dst_column_index][tile1_dst_row_index] = input_tiles_[1];
        input_tiles_[1] = nullptr;
    }
}

void tile_grid::drop_tiles(const data_types::tile_drop_list& drops)
{
    for(const auto& drop: drops)
    {
        if(const auto ptile = board_tiles_[drop.column_index][drop.src_row_index])
        {
            const auto dst_position = tile_coordinate_to_position(data_types::tile_coordinate{drop.column_index, drop.dst_row_index});
            board_tiles_[drop.column_index][drop.dst_row_index] = ptile;
            ptile->resetTransformation();
            ptile->translate(dst_position);
        }
    }
}

void tile_grid::merge_tiles(const data_types::tile_merge_list& merges)
{
    for(const auto& merge: merges)
    {
        const auto dst_position = tile_coordinate_to_position(merge.dst_tile_coordinate);

        for(const auto& src_tile_coordinate: merge.src_tile_coordinates)
        {
            if(auto& ptile = board_tiles_[src_tile_coordinate.x][src_tile_coordinate.y])
            {
                delete ptile;
            }
        }

        auto& dst_tile = addChild<tile>(drawables_);
        dst_tile.set_value(merge.dst_tile_value);
        dst_tile.translate(dst_position);
        dst_tile.set_alpha(1);
        board_tiles_[merge.dst_tile_coordinate.x][merge.dst_tile_coordinate.y] = &dst_tile;
    }
}

void tile_grid::update_input_tiles_positions()
{
    const auto dst_positions = get_input_tile_positions(input_x_offset_, input_rotation_);

    auto& animation = animations_.emplace_back();

    for(auto i = 0; i < 2; ++i)
    {
        animation.add_fixed_speed_translation
        (
            input_tiles_[i]->transformation().translation(),
            dst_positions[i],
            12,
            [](Magnum::Float, const Magnum::Vector2& translation, tile& object)
            {
                object.resetTransformation();
                object.translate(translation);
            },
            *input_tiles_[i]
        );
    }

    animation.player.play(std::chrono::system_clock::now().time_since_epoch());
}

void tile_grid::draw(const Magnum::Matrix3& /*transformationMatrix*/, SceneGraph::Camera2D& /*camera*/)
{
    for(auto& animation: animations_)
    {
        animation.player.advance(std::chrono::system_clock::now().time_since_epoch());
    }
    animations_.remove_if
    (
        [](const animation& a)
        {
            return a.player.state() == Magnum::Animation::State::Stopped;
        }
    );
}

} //namespace
