/*
Copyright 2018 - 2020 Florian Goujeon

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
#include "sdf_image.hpp"
#include "colors.hpp"
#include <libutil/array2d.hpp>

namespace libview
{

namespace
{
    Magnum::Vector2 tile_coordinate_to_position(const data_types::tile_coordinate& c)
    {
        return
        {
            -2.5f + c.column_index,
            -5.0f + c.row_index
        };
    }

    libutil::array2d<Magnum::Vector2, 2, 2> get_input_tile_positions
    (
        const data_types::input_layout& layout,
        const float y_offset
    )
    {
        auto positions = libutil::array2d<Magnum::Vector2, 2, 2>{};

        libutil::for_each_i
        (
            positions,
            [&](auto& pos, const int column_index, const int row_index)
            {
                //Get coordinate as indices
                const auto coord = get_tile_coordinate(layout, {column_index, row_index});

                //Convert to model coordinate
                pos = Magnum::Vector2
                {
                    coord.column_index - 2.5f,
                    coord.row_index - 0.5f + y_offset
                };
            }
        );

        //Center the tiles vertically if they are on the same line
        //if(tile0_y == tile1_y)
        //{
        //    tile0_y = tile1_y = 0.0f;
        //}

        return positions;
    }
}

tile_grid::tile_grid(SceneGraph::DrawableGroup2D& drawables, Object2D* parent):
    Object2D{parent},
    drawables_(drawables)
{
    //board corners
    {
        auto add_corner = [&](const float x, const float y, const auto rotation)
        {
            auto& obj = addChild<sdf_image>("/res/images/board_corner.tga", drawables_);
            obj.rotate(rotation);
            obj.scale({0.50f, 0.50f});
            obj.translate({x, y});
            obj.set_color(colors::light_gray);
            obj.set_outline_color(colors::dark_gray);
        };

        const auto shift  = 0.3f;
        const auto right  = 2.5f + shift;
        const auto left   = -right;
        const auto top    = 1.0f + shift;
        const auto bottom = -5.0f - shift;

        add_corner(right, top,    180.0_degf);
        add_corner(right, bottom,  90.0_degf);
        add_corner(left,  bottom,   0.0_degf);
        add_corner(left,  top,    -90.0_degf);
    }
}

bool tile_grid::is_animating() const
{
    return !animations_.empty();
}

void tile_grid::clear()
{
    libutil::for_each
    (
        next_input_tiles_,
        [&](auto& ptile)
        {
            delete ptile;
            ptile = nullptr;
        }
    );

    libutil::for_each
    (
        input_tiles_,
        [&](auto& ptile)
        {
            delete ptile;
            ptile = nullptr;
        }
    );

    libutil::for_each
    (
        board_tiles_,
        [&](auto& ptile)
        {
            delete ptile;
            ptile = nullptr;
        }
    );
}

void tile_grid::create_next_input(const data_types::input_tile_array& tiles)
{
    const auto animation_duration_s = 0.15f;

    auto& animation = animations_.emplace_back();

    //Create new next input and animate its creation.
    {
        const auto positions = get_input_tile_positions(input_layout_, 5.0f);

        libutil::for_each_i
        (
            tiles,
            [&](const auto& opt_tile, const int col, const int row)
            {
                if(opt_tile.has_value())
                {
                    next_input_tiles_[col][row] = &add_tile(opt_tile.value().value, positions[col][row]);
                    animation.add_alpha_transition(0, 0.4, animation_duration_s, *next_input_tiles_[col][row]);
                }
            }
        );
    }

    //Animate insertion of old next input.
    {
        const auto dst_positions = get_input_tile_positions(input_layout_, 3.0f);
        libutil::for_each_i
        (
            input_tiles_,
            [&](const auto& ptile, const int col, const int row)
            {
                if(ptile)
                {
                    animation.add_fixed_duration_translation
                    (
                        ptile->transformation().translation(),
                        dst_positions[col][row],
                        animation_duration_s,
                        *ptile
                    );
                    animation.add_alpha_transition(0.4, 1, animation_duration_s, *ptile);
                }
            }
        );
    }
}

void tile_grid::insert_next_input(const data_types::input_layout& layout)
{
    input_layout_ = layout;
    input_tiles_ = next_input_tiles_;

    libutil::for_each
    (
        next_input_tiles_,
        [&](auto& ptile)
        {
            ptile = nullptr;
        }
    );

    //Note: Animation is done in create_next_input().
}

void tile_grid::set_input_layout(const data_types::input_layout& layout)
{
    if(input_layout_ == layout)
    {
        return;
    }

    input_layout_ = layout;

    const auto dst_positions = get_input_tile_positions(layout, 3.0f);

    auto& animation = animations_.emplace_back();

    libutil::for_each_i
    (
        input_tiles_,
        [&](const auto ptile, const int col, const int row)
        {
            if(ptile)
            {
                animation.add_fixed_speed_translation
                (
                    ptile->transformation().translation(),
                    dst_positions[col][row],
                    20,
                    *ptile
                );
            }
        }
    );
}

void tile_grid::insert_input(const data_types::input_tile_coordinate_array& dst_coordinates)
{
    libutil::for_each_i
    (
        input_tiles_,
        [&](auto& ptile, const int col, const int row)
        {
            if(ptile)
            {
                const auto column_index = dst_coordinates[col][row].column_index;
                const auto row_index = dst_coordinates[col][row].row_index;
                std::swap
                (
                    board_tiles_[column_index][row_index],
                    ptile
                );
            }
        }
    );
}

void tile_grid::drop_tiles(const data_types::tile_drop_list& drops)
{
    auto& animation = animations_.emplace_back();

    /*
    When the input is in vertical position, we don't actually draw the tiles
    on rows #8 and #9, but half a row below. We must manager this case here.
    */
    const bool falling_from_vertical_input =
        drops.size() == 2 &&
        drops[0].src_row_index + drops[1].src_row_index == 8 + 9
    ;
    const auto src_position_extra_shift =
        falling_from_vertical_input ?
        Magnum::Vector2{0, -0.5} :
        Magnum::Vector2{0, 0}
    ;

    for(const auto& drop: drops)
    {
        auto& ptile = board_tiles_[drop.column_index][drop.src_row_index];

        const auto src_position =
            tile_coordinate_to_position(data_types::tile_coordinate{drop.column_index, drop.src_row_index}) +
            src_position_extra_shift
        ;

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
            auto& src_tile = *board_tiles_[src_tile_coordinate.column_index][src_tile_coordinate.row_index];

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
            board_tiles_[src_tile_coordinate.column_index][src_tile_coordinate.row_index] = nullptr;
        }

        //create destination tile
        auto& dst_tile = add_tile(merge.dst_tile_value, dst_position);
        board_tiles_[merge.dst_tile_coordinate.column_index][merge.dst_tile_coordinate.row_index] = &dst_tile;

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

void tile_grid::set_board_tiles(const data_types::board_tile_array& tiles)
{
    int column_index = 0;
    for(const auto& column_tiles: tiles)
    {
        int row_index = 0;
        for(const auto& opt_tile: column_tiles)
        {
            if(opt_tile.has_value())
            {
                auto ptile = &add_tile
                (
                    opt_tile.value().value,
                    tile_coordinate_to_position
                    (
                        data_types::tile_coordinate
                        {
                            column_index,
                            row_index
                        }
                    )
                );
                ptile->set_alpha(1);
                board_tiles_[column_index][row_index] = ptile;
            }
            ++row_index;
        }
        ++column_index;
    }
}

void tile_grid::advance(const time_point& now)
{
    if(!animations_.empty())
    {
        auto& animation = animations_.front();
        if(!animation.is_done())
        {
            animation.advance(now);
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
    t.scale({0.46f, 0.46f});
    t.translate(position);
    return t;
}

} //namespace
