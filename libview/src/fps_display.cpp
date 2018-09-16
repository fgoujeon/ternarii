/*
Copyright 2018 Florian Goujeon

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

#include "fps_display.hpp"

namespace libview
{

fps_display::fps_display
(
    SDL_Renderer& renderer,
    const SDL_Rect& area
):
    label_
    (
        renderer,
        "res/fonts/DejaVuSans.ttf",
        50,
        SDL_Color{0xff, 0xff, 0xff, 0xff},
        geometry::point
        {
            static_cast<double>(area.x),
            static_cast<double>(area.y)
        },
        area.w,
        area.h,
        "0",
        horizontal_alignment::left,
        vertical_alignment::center
    )
{
}

void fps_display::draw(const geometry::system& sys, const double ellapsed_time)
{
    cumulated_ellapsed_time_ += ellapsed_time;
    ++counter_;

    if(cumulated_ellapsed_time_ > 0.5)
    {
        const auto fps = static_cast<unsigned int>(counter_ / cumulated_ellapsed_time_);
        label_.set_text(std::to_string(fps) + "fps");

        cumulated_ellapsed_time_ = 0;
        counter_ = 0;
    }

    label_.draw(sys);
}

} //namespace view
