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

#include "next_input.hpp"
#include "number_tile.hpp"
#include "sdf_image_tile.hpp"
#include "../animation.hpp"
#include "../common.hpp"
#include <libres.hpp>
#include <libutil/overload.hpp>

namespace libview::objects
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

next_input::next_input
(
    Object2D& parent,
    features::drawable_group& drawables,
    animator& animator,
    tile_object_matrix& tile_objects,
    tile_object_matrix& input_tile_objects
):
    Object2D(&parent),
    drawables_(drawables),
    animator_(animator),
    tile_objects_(tile_objects),
    input_tile_objects_(input_tile_objects)
{
}

void next_input::create_next_input(const data_types::input_tile_matrix& tiles)
{
    const auto animation_duration_s = 0.2f;

    auto anim = animation{};

    //Animate insertion of current next input into input.
    {
        const auto dst_positions = get_input_tile_positions(input_tile_objects_, data_types::input_layout{}, 3.0f);

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
            input_tile_objects_,
            dst_positions
        );
    }

    //Create new next input and animate its creation.
    {
        const auto positions = get_input_tile_positions(tiles, data_types::input_layout{}, 5.0f);

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
            tile_objects_
        );
    }

    animator_.push(std::move(anim));
}

std::shared_ptr<Object2D> next_input::make_tile
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
            [&](const data_types::number_tile& tile) -> result_t
            {
                return std::make_shared<number_tile>(*this, drawables_, tile.value);
            },
            [&](const data_types::column_nullifier_tile&) -> result_t
            {
                return std::make_shared<sdf_image_tile>(*this, drawables_, libres::images::column_nullifier);
            },
            [&](const data_types::row_nullifier_tile&) -> result_t
            {
                return std::make_shared<sdf_image_tile>(*this, drawables_, libres::images::row_nullifier);
            },
            [&](const data_types::number_nullifier_tile&) -> result_t
            {
                return std::make_shared<sdf_image_tile>(*this, drawables_, libres::images::number_nullifier);
            }
        },
        tile
    );

    ptile->set_alpha(0.0f);
    ptile->scale({0.46f, 0.46f});
    ptile->translate(position);

    return ptile;
}

} //namespace
