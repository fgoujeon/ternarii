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
#include "number_tile.hpp"
#include "sdf_image.hpp"
#include "../colors.hpp"
#include <libutil/matrix.hpp>
#include <libutil/overload.hpp>

namespace libview::objects
{

namespace
{
    Magnum::Vector2 tile_coordinate_to_position(const data_types::tile_coordinate& c)
    {
        return
        {
            -2.5f + c.col,
            -5.0f + c.row
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
            [&](auto& pos, const int row, const int col)
            {
                //Get coordinate as indices
                const auto coord = get_tile_coordinate(layout, {row, col});

                //Convert to model coordinate
                pos = Magnum::Vector2
                {
                    coord.col - 2.5f,
                    coord.row - 0.5f + y_offset
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

tile_grid::tile_grid(Object2D& parent, features::drawable_group& drawables):
    Object2D{&parent},
    drawables_(drawables)
{
    //board corners
    {
        auto add_corner = [&](const float x, const float y, const auto rotation)
        {
            auto pobj = std::make_unique<sdf_image>
            (
                *this,
                drawables,
                "/res/images/board_corner.tga",
                sdf_image::style
                {
                    .color = colors::light_gray,
                    .outline_color = colors::dark_gray,
                    .outline_range = {0.5f, 0.4f}
                }
            );
            pobj->rotate(rotation);
            pobj->scale({0.50f, 0.50f});
            pobj->translate({x, y});

            board_corners_.push_back(std::move(pobj));
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
    return animator_.is_animating();
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

    auto anim = animation{};

    //Animate insertion of current next input into input.
    {
        const auto dst_positions = get_input_tile_positions(input_tiles_, input_layout_, 3.0f);

        libutil::for_each
        (
            [&](const auto& ptile, const auto& dst_position)
            {
                if(ptile)
                {
                    anim.add
                    (
                        tracks::fixed_duration_translation
                        {
                            ptile,
                            dst_position,
                            animation_duration_s
                        }
                    );
                    anim.add(tracks::alpha_transition{ptile, 1, animation_duration_s});
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
                    pnext_input_tile = make_tile(opt_tile.value(), position);
                    anim.add(tracks::alpha_transition{pnext_input_tile, 0.4, animation_duration_s});
                }
            },
            tiles,
            positions,
            next_input_tiles_
        );
    }

    animator_.push(std::move(anim));
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

    auto anim = animation{};

    libutil::for_each
    (
        [&](const auto ptile, const auto& dst_position)
        {
            if(!ptile)
            {
                return;
            }

            anim.add
            (
                tracks::fixed_speed_translation
                {
                    ptile,
                    dst_position,
                    20
                }
            );
        },
        input_tiles_,
        dst_positions
    );

    animator_.push(std::move(anim));
}

void tile_grid::drop_input_tiles(const data_types::input_tile_drop_list& drops)
{
    auto anim = animation{};

    for(const auto& drop: drops)
    {
        auto& ptile = libutil::at(input_tiles_, drop.input_coordinate.row, drop.input_coordinate.col);

        const auto dst_position = tile_coordinate_to_position(drop.board_coordinate);

        anim.add
        (
            tracks::fixed_speed_translation
            {
                ptile,
                dst_position,
                24
            }
        );

        libutil::at(board_tiles_, drop.board_coordinate.row, drop.board_coordinate.col) = ptile;
        ptile = nullptr;
    }

    animator_.push(std::move(anim));
    animator_.push(tracks::pause{0.05});
}

void tile_grid::drop_board_tiles(const data_types::board_tile_drop_list& drops)
{
    auto anim = animation{};

    for(const auto& drop: drops)
    {
        auto& ptile = libutil::at(board_tiles_, drop.src_row, drop.col);

        const auto dst_position = tile_coordinate_to_position(data_types::tile_coordinate{drop.dst_row, drop.col});

        anim.add
        (
            tracks::fixed_speed_translation
            {
                ptile,
                dst_position,
                24
            }
        );

        libutil::at(board_tiles_, drop.dst_row, drop.col) = ptile;
        ptile = nullptr;
    }

    animator_.push(std::move(anim));
    animator_.push(tracks::pause{0.05});
}

void tile_grid::nullify_tiles(const data_types::tile_coordinate_list& nullified_tile_coordinates)
{
    auto anim0 = animation{};
    auto anim1 = animation{};

    for(const auto& coord: nullified_tile_coordinates)
    {
        auto& ptile = libutil::at(board_tiles_, coord.row, coord.col);

        if(!ptile)
        {
            continue;
        }

        anim0.add(tracks::immediate_alpha_transition{ptile, 1});
        anim1.add(tracks::alpha_transition{ptile, 0, 0.4});

        ptile = nullptr;
    }

    animator_.push(std::move(anim0));
    animator_.push(std::move(anim1));
    animator_.push(tracks::pause{0.05});
}

void tile_grid::merge_tiles(const data_types::tile_merge_list& merges)
{
    auto anim0 = animation{};
    auto anim1 = animation{};

    for(const auto& merge: merges)
    {
        const auto dst_position = tile_coordinate_to_position(merge.dst_tile_coordinate);

        for(const auto& src_tile_coordinate: merge.src_tile_coordinates) //for each source tile
        {
            auto& psrc_tile = libutil::at(board_tiles_, src_tile_coordinate.row, src_tile_coordinate.col);

            //first, translate source tile toward position of destination tile
            anim0.add
            (
                tracks::fixed_speed_translation
                {
                    psrc_tile,
                    dst_position,
                    6
                }
            );

            //then, make it disappear with a fade out
            anim1.add
            (
                tracks::alpha_transition
                {
                    psrc_tile, 0, 0.2
                }
            );

            //remove the tile object from the matrix so that it is deleted once
            //the animation ends
            psrc_tile = nullptr;
        }

        //create destination tile
        auto pdst_tile = make_tile(data_types::tiles::number{merge.dst_tile_value}, dst_position);
        libutil::at(board_tiles_, merge.dst_tile_coordinate.row, merge.dst_tile_coordinate.col) = pdst_tile;

        //make destination tile appear with a fade in
        anim1.add(tracks::alpha_transition{pdst_tile, 1, 0.2});
    }

    animator_.push(std::move(anim0));
    animator_.push(std::move(anim1));
    animator_.push(tracks::pause{0.05});
}

void tile_grid::mark_tiles_for_nullification(const data_types::tile_coordinate_list& tile_coordinates)
{
    auto anim = animation{};

    //Unmark all board tiles
    for(auto& ptile: board_tiles_)
    {
        if(ptile)
        {
            anim.add(tracks::immediate_alpha_transition{ptile, 1});
        }
    }

    //Mark given board tiles
    for(const auto& coords: tile_coordinates)
    {
        auto& ptile = libutil::at(board_tiles_, coords.row, coords.col);
        if(ptile)
        {
            anim.add(tracks::immediate_alpha_transition{ptile, 0.5});
        }
    }

    animator_.push(std::move(anim));
}

void tile_grid::set_board_tiles(const data_types::board_tile_array& tiles)
{
    libutil::for_each_ij
    (
        [&](const auto& opt_tile, const int row, const int col)
        {
            if(!opt_tile.has_value())
            {
                return;
            }

            const auto position = tile_coordinate_to_position({row, col});

            auto ptile = make_tile(opt_tile.value(), position);
            ptile->set_alpha(1);
            libutil::at(board_tiles_, row, col) = ptile;
        },
        tiles
    );
}

void tile_grid::advance(const libutil::time_point& now)
{
    animator_.advance(now);
}

std::shared_ptr<Object2D> tile_grid::make_tile
(
    const data_types::tile& tile,
    const Magnum::Vector2& position
)
{
    using result_t = std::shared_ptr<Object2D>;

    auto ptile = std::visit
    (
        libutil::overload
        {
            [&](const data_types::tiles::number& tile) -> result_t
            {
                return std::make_shared<number_tile>(*this, drawables_, tile.value);
            },
            [&](const data_types::tiles::column_nullifier&) -> result_t
            {
                return std::make_shared<sdf_image_tile>(*this, drawables_, "/res/images/column_nullifier.tga");
            },
            [&](const data_types::tiles::row_nullifier&) -> result_t
            {
                return std::make_shared<sdf_image_tile>(*this, drawables_, "/res/images/row_nullifier.tga");
            },
            [&](const data_types::tiles::number_nullifier&) -> result_t
            {
                return std::make_shared<sdf_image_tile>(*this, drawables_, "/res/images/number_nullifier.tga");
            }
        },
        tile
    );

    ptile->scale({0.46f, 0.46f});
    ptile->translate(position);

    return ptile;
}

} //namespace
