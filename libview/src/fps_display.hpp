/*
Copyright 2018 - 2019 Florian Goujeon

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

#ifndef LIBVIEW_FPS_DISPLAY_HPP
#define LIBVIEW_FPS_DISPLAY_HPP

#include "geometry.hpp"
#include "label.hpp"
#include <libsdl.hpp>

namespace libview
{

class fps_display
{
    public:
        fps_display(SDL_Renderer& renderer, const SDL_Rect& area);

        void draw(const geometry::system& sys, const double ellapsed_time);

    private:
        label label_;
        double cumulated_ellapsed_time_ = 0;
        unsigned int counter_ = 0;
};

} //namespace libview

#endif
