/*
Copyright 2018 - 2021 Florian Goujeon

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

#ifndef LIBVIEW_OBJECTS_DEBUG_GRID_HPP
#define LIBVIEW_OBJECTS_DEBUG_GRID_HPP

#include "square.hpp"
#include "../common.hpp"

namespace libview::objects
{

class debug_grid: public object2d
{
    public:
        debug_grid(object2d& parent, features::drawable_group& drawables):
            object2d{&parent}
        {
            const auto line_color = 0x111111_rgbf;
            const auto line_thickness = 0.03f;

            //Center point
            {
                auto ppoint = std::make_unique<square>(*this, drawables, line_color);
                ppoint->scale({line_thickness * 4, line_thickness * 4});
                squares.push_back(std::move(ppoint));
            }

            //Horizontal lines
            for(int i = 0; i < 17; ++i)
            {
                auto pline = std::make_unique<square>(*this, drawables, line_color);
                pline->scale({4.5f, line_thickness});
                pline->translate({0.0f, -8.0f + i});
                squares.push_back(std::move(pline));
            }

            //Vertical lines
            for(int i = 0; i < 9; ++i)
            {
                auto pline = std::make_unique<square>(*this, drawables, line_color);
                pline->scale({line_thickness, 8.0f});
                pline->translate({-4.0f + i, 0.0f});
                squares.push_back(std::move(pline));
            }
        }

    private:
        std::vector<std::unique_ptr<square>> squares;
};

} //namespace

#endif
