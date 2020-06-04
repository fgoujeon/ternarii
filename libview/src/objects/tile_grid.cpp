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
#include "tile_grid_detail/common.hpp"
#include "number_tile.hpp"
#include "sdf_image.hpp"
#include "../colors.hpp"
#include <libres.hpp>
#include <libutil/matrix.hpp>
#include <libutil/overload.hpp>

namespace libview::objects
{

namespace
{
    Magnum::Vector2 tile_coordinate_to_position(const libutil::matrix_coordinate& c)
    {
        return
        {
            -2.5f + c.col,
            -5.0f + c.row
        };
    }

    const auto tile_move_interpolator = Magnum::Animation::ease
    <
        Magnum::Vector2,
        Magnum::Math::lerp,
        Magnum::Animation::Easing::cubicOut
    >();
}

tile_grid::tile_grid
(
    Object2D& parent,
    features::drawable_group& drawables,
    features::animable_group& animables
):
    Object2D{&parent},
    features::animable(*this, &animables),
    drawables_(drawables),
    next_input_(*this, drawables, animables),
    input_(*this, animables, input_tiles_)
{
    //board corners
    {
        auto add_corner = [&](const float x, const float y, const auto rotation)
        {
            auto pobj = std::make_unique<sdf_image>
            (
                *this,
                drawables,
                libres::images::board_corner,
                sdf_image::style
                {
                    .color = colors::light_gray,
                    .outline_color = colors::black,
                    .outline_range = {0.5f, 0.3f}
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

    clear_matrix(next_input_.get_tile_objects());
    clear_matrix(input_tiles_);
    clear_matrix(board_tiles_);
}

void tile_grid::create_next_input(const data_types::input_tile_matrix& tiles)
{
    next_input_.create_tiles(tiles);
}

void tile_grid::insert_next_input(const data_types::input_layout& layout)
{
    input_.insert_next_input(next_input_.get_tile_objects(), layout);
}

void tile_grid::set_input_layout(const data_types::input_layout& layout)
{
    input_.set_input_layout(layout);
}

void tile_grid::drop_input_tiles(const data_types::input_tile_drop_list& drops)
{
    auto anim = animation{};

    for(const auto& drop: drops)
    {
        auto& ptile = at(input_tiles_, drop.input_coordinate);

        const auto dst_position = tile_coordinate_to_position(drop.board_coordinate);

        anim.add
        (
            tracks::fixed_speed_translation
            {
                ptile,
                dst_position,
                22
            }
        );

        at(board_tiles_, drop.board_coordinate) = ptile;
        ptile = nullptr;
    }

    animator_.push(tracks::closure{[this]{next_input_.suspend();}});
    animator_.push(tracks::closure{[this]{input_.suspend();}});
    animator_.push(std::move(anim));
    animator_.push(tracks::pause{0.05});
    animator_.push(tracks::closure{[this]{input_.resume();}});
    animator_.push(tracks::closure{[this]{next_input_.resume();}});
}

void tile_grid::drop_board_tiles(const data_types::board_tile_drop_list& drops)
{
    auto anim = animation{};

    for(const auto& drop: drops)
    {
        auto& ptile = at(board_tiles_, drop.col, drop.src_row);

        const auto dst_position = tile_coordinate_to_position(libutil::matrix_coordinate{drop.col, drop.dst_row});

        anim.add
        (
            tracks::fixed_speed_translation
            {
                ptile,
                dst_position,
                22
            }
        );

        at(board_tiles_, drop.col, drop.dst_row) = ptile;
        ptile = nullptr;
    }

    animator_.push(tracks::closure{[this]{next_input_.suspend();}});
    animator_.push(tracks::closure{[this]{input_.suspend();}});
    animator_.push(std::move(anim));
    animator_.push(tracks::pause{0.05});
    animator_.push(tracks::closure{[this]{input_.resume();}});
    animator_.push(tracks::closure{[this]{next_input_.resume();}});
}

void tile_grid::nullify_tiles(const libutil::matrix_coordinate_list& nullified_tile_coordinates)
{
    auto anim0 = animation{};
    auto anim1 = animation{};

    for(const auto& coord: nullified_tile_coordinates)
    {
        auto& ptile = at(board_tiles_, coord);

        if(!ptile)
        {
            continue;
        }

        anim0.add(tracks::alpha_transition{ptile, 1, 0});
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
            auto& psrc_tile = at(board_tiles_, src_tile_coordinate);

            //first, translate source tile toward position of destination tile
            anim0.add
            (
                tracks::fixed_speed_translation
                {
                    psrc_tile,
                    dst_position,
                    4.5,
                    tile_move_interpolator
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
        auto pdst_tile = make_tile(data_types::number_tile{merge.dst_tile_value}, dst_position);
        at(board_tiles_, merge.dst_tile_coordinate) = pdst_tile;

        //make destination tile appear with a fade in
        anim1.add(tracks::alpha_transition{pdst_tile, 1, 0.2});
    }

    animator_.push(tracks::closure{[this]{next_input_.suspend();}});
    animator_.push(tracks::closure{[this]{input_.suspend();}});
    animator_.push(std::move(anim0));
    animator_.push(std::move(anim1));
    animator_.push(tracks::closure{[this]{input_.resume();}});
    animator_.push(tracks::closure{[this]{next_input_.resume();}});
}

void tile_grid::mark_tiles_for_nullification(const libutil::matrix_coordinate_list& tile_coordinates)
{
    auto anim = animation{};

    //Unmark all board tiles
    for(auto& ptile: board_tiles_)
    {
        if(ptile)
        {
            anim.add(tracks::alpha_transition{ptile, 1, 0});
        }
    }

    //Mark given board tiles
    for(const auto& coords: tile_coordinates)
    {
        auto& ptile = at(board_tiles_, coords);
        if(ptile)
        {
            anim.add(tracks::alpha_transition{ptile, 0.5, 0});
        }
    }

    animator_.push(std::move(anim));
}

void tile_grid::set_board_tiles(const data_types::board_tile_matrix& tiles)
{
    libutil::for_each_colrow
    (
        [&](const auto& opt_tile, const int col, const int row)
        {
            if(!opt_tile.has_value())
            {
                return;
            }

            const auto position = tile_coordinate_to_position({col, row});

            auto ptile = make_tile(opt_tile.value(), position);
            ptile->set_alpha(1);
            at(board_tiles_, col, row) = ptile;
        },
        tiles
    );
}

void tile_grid::advance(const libutil::time_point& now, const float /*elapsed_s*/)
{
    animator_.advance(now);
}

std::shared_ptr<Object2D> tile_grid::make_tile
(
    const data_types::tile& tile,
    const Magnum::Vector2& position
)
{
    auto ptile = tile_grid_detail::make_tile_object(*this, drawables_, tile);
    ptile->translate(position);
    return ptile;
}

} //namespace
