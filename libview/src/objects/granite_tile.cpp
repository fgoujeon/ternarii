/*
Copyright 2018 - 2022 Florian Goujeon

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

#include "granite_tile.hpp"
#include "../colors.hpp"
#include <libres.hpp>

namespace libview::objects
{

namespace
{
    std::unique_ptr<rounded_rectangle> make_inner_square
    (
        object2d& parent,
        features::drawable_group& drawables,
        const int level
    )
    {
        auto psquare = std::make_unique<rounded_rectangle>
        (
            parent,
            drawables,
            rounded_rectangle::style
            {
                .color = 0x00000000_rgbaf,
                .outline_color = colors::dark_gray,
                .outline_thickness = 0.1f,
                .radius = 0.6f
            }
        );
        const auto scaling = 0.9f - level / 4.0f;
        psquare->setScaling({scaling, scaling});
        return psquare;
    }

    std::vector<std::unique_ptr<rounded_rectangle>> make_inner_squares
    (
        object2d& parent,
        features::drawable_group& drawables,
        const int thickness
    )
    {
        auto squares = std::vector<std::unique_ptr<rounded_rectangle>>{};
        for(auto i = 0; i < thickness - 1; ++i)
        {
            squares.push_back(make_inner_square(parent, drawables, i));
        }
        return squares;
    }
}

granite_tile::granite_tile
(
    object2d& parent,
    features::drawable_group& drawables,
    const int thickness
):
    object2d(&parent),
    square_
    (
        *this,
        drawables,
        rounded_rectangle::style
        {
            .color = colors::light_gray,
            .radius = 0.6f
        }
    ),
    inner_squares_(make_inner_squares(*this, drawables, thickness))
{
}

} //namespace
