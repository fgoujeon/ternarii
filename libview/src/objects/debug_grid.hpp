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

#ifndef LIBVIEW_OBJECTS_DEBUG_GRID_HPP
#define LIBVIEW_OBJECTS_DEBUG_GRID_HPP

#include "square.hpp"
#include "../common.hpp"

namespace libview::objects
{

class debug_grid: public Object2D
{
    public:
        debug_grid(Object2D& parent, features::drawable_group& drawables):
            Object2D{&parent}
        {
            for(int x = 0; x < 9; ++x)
            {
                for(int y = 0; y < 16; ++y)
                {
                    auto psquare = std::make_unique<square>(*this, drawables, 0x111111_rgbf);
                    psquare->scale({0.48f, 0.48f});
                    psquare->translate({-4.0f + x, -7.5f + y});
                    squares_.push_back(std::move(psquare));
                }
            }
        }

    private:
        std::vector<std::unique_ptr<square>> squares_;
};

} //namespace

#endif
