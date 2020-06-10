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
#include "common.hpp"
#include "../number_tile.hpp"
#include "../sdf_image_tile.hpp"
#include "../../animation.hpp"
#include "../../common.hpp"
#include <libres.hpp>
#include <libutil/overload.hpp>

namespace libview::objects::tile_grid_detail
{

next_input::next_input
(
    Object2D& parent,
    features::drawable_group& drawables,
    features::animable_group& animables
):
    Object2D(&parent),
    features::animable(*this, &animables),
    drawables_(drawables)
{
}

void next_input::create_tiles(const data_types::input_tile_matrix& tiles)
{
    const auto animation_duration_s = 0.2f;

    auto anim = animation::animation{};

    //Make tiles
    libutil::for_each_colrow
    (
        [&](const auto& opt_tile, auto& pnext_input_tile, const int col, const int row)
        {
            if(opt_tile.has_value())
            {
                const auto x = (col - 0.5f) / 1.28f;
                const auto y = (row - 0.5f) / 1.28f;

                pnext_input_tile = make_tile_object(*this, drawables_, opt_tile.value());
                pnext_input_tile->set_alpha(0);
                pnext_input_tile->setScaling({0.36f, 0.36f});
                pnext_input_tile->setTranslation({x, y});
            }
        },
        tiles,
        tile_objects_
    );

    //Animate next input creation.
    for(auto& ptile_object: tile_objects_)
    {
        if(ptile_object)
        {
            anim.add(animation::tracks::alpha_transition{ptile_object, 0.4f, animation_duration_s});
        }
    }

    animator_.push(std::move(anim));
}

input_tile_object_matrix next_input::release_tile_objects()
{
    auto out = tile_objects_;
    for(auto& ptile_object: tile_objects_)
    {
        ptile_object.reset();
    }
    return out;
}

void next_input::suspend()
{
    suspended_ = true;
}

void next_input::resume()
{
    suspended_ = false;
}

void next_input::advance(const libutil::time_point& now, float /*elapsed_s*/)
{
    if(!suspended_)
    {
        animator_.advance(now);
    }
}

} //namespace
