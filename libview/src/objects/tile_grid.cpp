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
}

tile_grid::tile_grid
(
    object2d& parent,
    features::drawable_group& drawables,
    features::animable_group& animables,
    animation::animator& animator,
    const drop_request_callback& drop_cb,
    const input_layout_change_callback& layout_cb
):
    object2d{&parent},
    features::animable(*this, &animables),
    drawables_(drawables),
    animables_(animables),
    animator_(animator),
    next_input_(*this, drawables, animables),
    input_(*this, animables, drop_cb, layout_cb)
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
                    .outline_color = colors::dark_gray,
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

    next_input_.setTranslation({0.0f, 5.0f});
    input_.setTranslation({0.0f, 3.0f});
}

bool tile_grid::is_animating() const
{
    return animator_.is_animating();
}

const data_types::input_layout& tile_grid::get_input_layout() const
{
    return input_.get_layout();
}

void tile_grid::clear()
{
    next_input_.release_tile_objects();
    input_.release_tile_objects();

    for(auto& ptile: board_tiles_)
    {
        ptile.reset();
    }
}

void tile_grid::create_next_input(const data_types::input_tile_matrix& tiles)
{
    next_input_.create_tiles(tiles);
}

void tile_grid::insert_next_input()
{
    input_.set_tiles(next_input_.release_tile_objects());
}

void tile_grid::drop_input_tiles(const data_types::input_tile_drop_list& drops)
{
    auto anim = animation::animation{};

    //Retrieve tile objects
    const auto input_tile_objects = input_.release_tile_objects();
    for(auto& ptile: input_tile_objects)
    {
        if(ptile)
        {
            ptile->setParentKeepTransformation(this);
        }
    }

    for(const auto& drop: drops)
    {
        const auto& ptile = at(input_tile_objects, drop.input_coordinate);

        const auto dst_position = tile_coordinate_to_position(drop.board_coordinate);

        anim.add
        (
            animation::tracks::fixed_speed_translation
            {
                ptile,
                dst_position,
                22
            }
        );

        at(board_tiles_, drop.board_coordinate) = ptile;
    }

    animator_.push(animation::tracks::closure{[this]{next_input_.suspend();}});
    animator_.push(animation::tracks::closure{[this]{input_.suspend();}});
    animator_.push(std::move(anim));
    animator_.push(animation::tracks::pause{0.05});
    animator_.push(animation::tracks::closure{[this]{input_.resume();}});
    animator_.push(animation::tracks::closure{[this]{next_input_.resume();}});
}

void tile_grid::drop_board_tiles(const data_types::board_tile_drop_list& drops)
{
    auto anim = animation::animation{};

    for(const auto& drop: drops)
    {
        auto& ptile = at(board_tiles_, drop.col, drop.src_row);

        const auto dst_position = tile_coordinate_to_position(libutil::matrix_coordinate{drop.col, drop.dst_row});

        anim.add
        (
            animation::tracks::fixed_speed_translation
            {
                ptile,
                dst_position,
                22
            }
        );

        at(board_tiles_, drop.col, drop.dst_row) = ptile;
        ptile = nullptr;
    }

    animator_.push(animation::tracks::closure{[this]{next_input_.suspend();}});
    animator_.push(animation::tracks::closure{[this]{input_.suspend();}});
    animator_.push(std::move(anim));
    animator_.push(animation::tracks::pause{0.05});
    animator_.push(animation::tracks::closure{[this]{input_.resume();}});
    animator_.push(animation::tracks::closure{[this]{next_input_.resume();}});
}

void tile_grid::nullify_tiles(const libutil::matrix_coordinate_list& nullified_tile_coordinates)
{
    auto anim0 = animation::animation{};
    auto anim1 = animation::animation{};

    for(const auto& coord: nullified_tile_coordinates)
    {
        auto& ptile = at(board_tiles_, coord);

        if(!ptile)
        {
            continue;
        }

        anim0.add(animation::tracks::alpha_transition{ptile, 1, 0});
        anim1.add(animation::tracks::alpha_transition{ptile, 0, 0.4});

        ptile = nullptr;
    }

    animator_.push(animation::tracks::closure{[this]{next_input_.suspend();}});
    animator_.push(animation::tracks::closure{[this]{input_.suspend();}});
    animator_.push(std::move(anim0));
    animator_.push(std::move(anim1));
    animator_.push(animation::tracks::pause{0.05});
    animator_.push(animation::tracks::closure{[this]{input_.resume();}});
    animator_.push(animation::tracks::closure{[this]{next_input_.resume();}});
}

void tile_grid::merge_tiles(const data_types::tile_merge_list& merges)
{
    auto anim0 = animation::animation{};
    auto anim1 = animation::animation{};

    for(const auto& merge: merges)
    {
        const auto dst_position = tile_coordinate_to_position(merge.dst_tile_coordinate);

        for(const auto& src_tile_coordinate: merge.src_tile_coordinates) //for each source tile
        {
            auto& psrc_tile = at(board_tiles_, src_tile_coordinate);

            //first, translate source tile toward position of destination tile
            anim0.add
            (
                animation::tracks::fixed_speed_translation
                {
                    psrc_tile,
                    dst_position,
                    4.5,
                    animation::get_cubic_out_position_interpolator()
                }
            );

            //then, make it disappear with a fade out
            anim1.add
            (
                animation::tracks::alpha_transition
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
        pdst_tile->set_alpha(0);
        at(board_tiles_, merge.dst_tile_coordinate) = pdst_tile;

        //make destination tile appear with a fade in
        anim1.add(animation::tracks::alpha_transition{pdst_tile, 1, 0.2});
    }

    animator_.push(animation::tracks::closure{[this]{next_input_.suspend();}});
    animator_.push(animation::tracks::closure{[this]{input_.suspend();}});
    animator_.push(std::move(anim0));
    animator_.push(std::move(anim1));
    animator_.push(animation::tracks::closure{[this]{input_.resume();}});
    animator_.push(animation::tracks::closure{[this]{next_input_.resume();}});
}

void tile_grid::mark_tiles_for_nullification(const libutil::matrix_coordinate_list& tile_coordinates)
{
    auto anim = animation::animation{};

    //Unmark all board tiles
    for(auto& ptile: board_tiles_)
    {
        if(ptile)
        {
            anim.add(animation::tracks::alpha_transition{ptile, 1, 0});
        }
    }

    //Mark given board tiles
    for(const auto& coords: tile_coordinates)
    {
        auto& ptile = at(board_tiles_, coords);
        if(ptile)
        {
            anim.add(animation::tracks::alpha_transition{ptile, 0.5, 0});
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
            at(board_tiles_, col, row) = ptile;
        },
        tiles
    );
}

void tile_grid::advance(const std::chrono::steady_clock::time_point& now, const float /*elapsed_s*/)
{
    animator_.advance(now);
}

void tile_grid::handle_button_press(data_types::move_button button)
{
    input_.handle_button_press(button);
}

void tile_grid::handle_button_release(data_types::move_button button)
{
    input_.handle_button_release(button);
}

std::shared_ptr<object2d> tile_grid::make_tile
(
    const data_types::tile& tile,
    const Magnum::Vector2& position
)
{
    auto ptile = tile_grid_detail::make_tile_object(*this, drawables_, animables_, tile);
    ptile->setScaling({0.46f, 0.46f});
    ptile->setTranslation(position);
    return ptile;
}

} //namespace
