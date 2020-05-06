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
#include <libutil/matrix.hpp>

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

    template<class TileMatrix>
    libutil::matrix<Magnum::Vector2, 2, 2> get_input_tile_positions
    (
        const TileMatrix& tiles,
        const data_types::input_layout& layout,
        const float y_offset
    )
    {
        auto positions = libutil::matrix<Magnum::Vector2, 2, 2>{};

        libutil::for_each_ij
        (
            [&](auto& pos, const int col, const int row)
            {
                //Get coordinate as indices
                const auto coord = get_tile_coordinate(layout, {col, row});

                //Convert to model coordinate
                pos = Magnum::Vector2
                {
                    coord.column_index - 2.5f,
                    coord.row_index - 0.5f + y_offset
                };
            },
            positions
        );

        //Center the tiles vertically if they are on the same line
        const auto on_same_line = [&]
        {
            auto on_same_line = true;
            auto opt_y = std::optional<float>{};
            libutil::for_each
            (
                [&](const auto& pos, const auto& ptile)
                {
                    if(!ptile)
                    {
                        return;
                    }

                    if(!opt_y)
                    {
                        opt_y = pos.y();
                        return;
                    }

                    if(pos.y() != opt_y)
                    {
                        on_same_line = false;
                    }
                },
                positions,
                tiles
            );

            return on_same_line;
        }();

        if(on_same_line)
        {
            for(auto& pos: positions)
            {
                pos.y() = y_offset;
            }
        }

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
    const auto clear_matrix = [](auto& matrix)
    {
        for(auto& ptile: matrix)
        {
            ptile.reset();
        }
    };

    clear_matrix(next_input_tiles_);
    clear_matrix(input_tiles_);
    clear_matrix(board_tiles_);
}

void tile_grid::create_next_input(const data_types::input_tile_array& tiles)
{
    const auto animation_duration_s = 0.15f;

    auto& animation = animations_.emplace_back();

    //Animate insertion of current next input into input.
    {
        const auto dst_positions = get_input_tile_positions(input_tiles_, input_layout_, 3.0f);

        libutil::for_each
        (
            [&](const auto& ptile, const auto& dst_position)
            {
                if(ptile)
                {
                    animation.add_fixed_duration_translation
                    (
                        ptile->transformation().translation(),
                        dst_position,
                        animation_duration_s,
                        ptile
                    );
                    animation.add_alpha_transition(0.4, 1, animation_duration_s, ptile);
                }
            },
            input_tiles_,
            dst_positions
        );
    }

    //Create new next input and animate its creation.
    {
        const auto positions = get_input_tile_positions(tiles, input_layout_, 5.0f);

        libutil::for_each
        (
            [&](const auto& opt_tile, const auto& position, auto& pnext_input_tile)
            {
                if(opt_tile.has_value())
                {
                    pnext_input_tile = make_tile(opt_tile.value().value, position);
                    animation.add_alpha_transition(0, 0.4, animation_duration_s, pnext_input_tile);
                }
            },
            tiles,
            positions,
            next_input_tiles_
        );
    }
}

void tile_grid::insert_next_input(const data_types::input_layout& layout)
{
    input_layout_ = layout;
    input_tiles_ = next_input_tiles_;

    for(auto& ptile: next_input_tiles_)
    {
        ptile = nullptr;
    }

    //Note: Animation is done in create_next_input().
}

void tile_grid::set_input_layout(const data_types::input_layout& layout)
{
    if(input_layout_ == layout)
    {
        return;
    }

    input_layout_ = layout;

    const auto dst_positions = get_input_tile_positions(input_tiles_, layout, 3.0f);

    auto& animation = animations_.emplace_back();

    libutil::for_each
    (
        [&](const auto ptile, const auto& dst_position)
        {
            if(!ptile)
            {
                return;
            }

            const auto src_position = ptile->transformation().translation();

            if(src_position == dst_position)
            {
                return;
            }

            animation.add_fixed_speed_translation
            (
                src_position,
                dst_position,
                20,
                ptile
            );
        },
        input_tiles_,
        dst_positions
    );
}

void tile_grid::drop_input_tiles(const data_types::input_tile_drop_list& drops)
{
    auto& animation = animations_.emplace_back();

    for(const auto& drop: drops)
    {
        auto& ptile = libutil::at(input_tiles_, drop.input_coordinate.column_index, drop.input_coordinate.row_index);

        const auto src_position = ptile->transformation().translation();
        const auto dst_position = tile_coordinate_to_position(drop.board_coordinate);

        animation.add_fixed_speed_translation
        (
            src_position,
            dst_position,
            24,
            ptile
        );

        libutil::at(board_tiles_, drop.board_coordinate.column_index, drop.board_coordinate.row_index) = ptile;
        ptile = nullptr;
    }
}

void tile_grid::drop_board_tiles(const data_types::board_tile_drop_list& drops)
{
    auto& animation = animations_.emplace_back();

    for(const auto& drop: drops)
    {
        auto& ptile = libutil::at(board_tiles_, drop.column_index, drop.src_row_index);

        const auto src_position = tile_coordinate_to_position(data_types::tile_coordinate{drop.column_index, drop.src_row_index});
        const auto dst_position = tile_coordinate_to_position(data_types::tile_coordinate{drop.column_index, drop.dst_row_index});

        animation.add_fixed_speed_translation
        (
            src_position,
            dst_position,
            24,
            ptile
        );

        libutil::at(board_tiles_, drop.column_index, drop.dst_row_index) = ptile;
        ptile = nullptr;
    }
}

void tile_grid::merge_tiles(const data_types::tile_merge_list& merges)
{
    auto& animation0 = animations_.emplace_back();
    auto& animation1 = animations_.emplace_back();

    for(const auto& merge: merges)
    {
        const auto dst_position = tile_coordinate_to_position(merge.dst_tile_coordinate);

        for(const auto& src_tile_coordinate: merge.src_tile_coordinates) //for each source tile
        {
            const auto src_position = tile_coordinate_to_position(src_tile_coordinate);
            const auto& psrc_tile = libutil::at(board_tiles_, src_tile_coordinate.column_index, src_tile_coordinate.row_index);

            //first, translate source tile toward position of destination tile
            if(dst_position != src_position)
            {
                animation0.add_fixed_speed_translation
                (
                    src_position,
                    dst_position,
                    6,
                    psrc_tile
                );
            }

            //then, make it disappear with a fade out
            animation1.add_alpha_transition(1, 0, 0.2, psrc_tile);
        }

        //create destination tile
        auto pdst_tile = make_tile(merge.dst_tile_value, dst_position);
        libutil::at(board_tiles_, merge.dst_tile_coordinate.column_index, merge.dst_tile_coordinate.row_index) = pdst_tile;

        //make destination tile appear with a fade in
        animation1.add_alpha_transition(0, 1, 0.2, pdst_tile);
    }
}

void tile_grid::set_board_tiles(const data_types::board_tile_array& tiles)
{
    libutil::for_each_ij
    (
        [&](const auto& opt_tile, const int col, const int row)
        {
            if(opt_tile.has_value())
            {
                auto ptile = make_tile
                (
                    opt_tile.value().value,
                    tile_coordinate_to_position
                    (
                        data_types::tile_coordinate
                        {
                            col,
                            row
                        }
                    )
                );
                ptile->set_alpha(1);
                libutil::at(board_tiles_, col, row) = ptile;
            }
        },
        tiles
    );
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

std::shared_ptr<tile> tile_grid::make_tile
(
    const int value,
    const Magnum::Vector2& position
)
{
    auto t = std::make_shared<tile>(value, drawables_, this);
    t->scale({0.46f, 0.46f});
    t->translate(position);
    return t;
}

} //namespace
