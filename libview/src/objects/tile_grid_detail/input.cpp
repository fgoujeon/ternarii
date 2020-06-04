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

#include "input.hpp"
#include "../number_tile.hpp"
#include "../sdf_image_tile.hpp"
#include "../../animation.hpp"
#include "../../common.hpp"
#include <libres.hpp>
#include <libutil/overload.hpp>

namespace libview::objects::tile_grid_detail
{

namespace
{
    template<class TileMatrix>
    libutil::matrix<Magnum::Vector2, 2, 2> get_input_tile_positions
    (
        const TileMatrix& tiles,
        const data_types::input_layout& layout,
        const float y_offset
    )
    {
        auto positions = libutil::matrix<Magnum::Vector2, 2, 2>{};

        libutil::for_each_colrow
        (
            [&](auto& pos, const int col, const int row)
            {
                //Get coordinate as indices
                const auto coord = get_tile_coordinate(layout, {col, row});

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

    const auto tile_move_interpolator = Magnum::Animation::ease
    <
        Magnum::Vector2,
        Magnum::Math::lerp,
        Magnum::Animation::Easing::cubicOut
    >();
}

input::input
(
    Object2D& parent,
    features::animable_group& animables
):
    Object2D(&parent),
    features::animable(*this, &animables)
{
}

void input::insert_next_input
(
    const input_tile_object_matrix& next_input_tile_objects,
    const data_types::input_layout& layout
)
{
    tile_objects_ = next_input_tile_objects;
    layout_ = layout;

    const auto animation_duration_s = 0.2f;

    auto anim = animation{};

    //Animate insertion of current next input into input.
    {
        const auto dst_positions = get_input_tile_positions(tile_objects_, data_types::input_layout{}, 3.0f);

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
                            animation_duration_s,
                            tile_move_interpolator
                        }
                    );
                    anim.add(tracks::alpha_transition{ptile, 1, animation_duration_s});
                }
            },
            tile_objects_,
            dst_positions
        );
    }

    animator_.push(std::move(anim));
}

void input::set_input_layout(const data_types::input_layout& layout)
{
    if(layout_ == layout)
    {
        return;
    }

    layout_ = layout;

    const auto dst_positions = get_input_tile_positions(tile_objects_, layout, 3.0f);

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
        tile_objects_,
        dst_positions
    );

    animator_.push(std::move(anim));
}

input_tile_object_matrix input::release_tile_objects()
{
    auto out = tile_objects_;
    for(auto& ptile_object: tile_objects_)
    {
        ptile_object.reset();
    }
    return out;
}

void input::suspend()
{
    suspended_ = true;
}

void input::resume()
{
    suspended_ = false;
}

void input::advance(const libutil::time_point& now, float /*elapsed_s*/)
{
    if(!suspended_)
    {
        animator_.advance(now);
    }
}

} //namespace
