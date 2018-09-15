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

#ifndef LIBVIEW_SYSTEM_HPP
#define LIBVIEW_SYSTEM_HPP

#include "point.hpp"
#include <libsdl.hpp>

namespace libview
{

struct system
{
    point origin; //in pixels
    double unit = 1; //in pixels, for both x and y axes
};

inline bool operator==(const system& l, const system& r)
{
    return l.origin == r.origin && l.unit == r.unit;
}

inline bool operator!=(const system& l, const system& r)
{
    return !(l == r);
}

/*
Convert a rect with given system coordinates into a rect with window system
coordinates.
Note: Window system is system{point{0, 0}, 1}.
*/
inline
SDL_Rect to_window_system_rect(const system& sys, const SDL_Rect& r)
{
    return SDL_Rect
    {
        static_cast<int>(sys.unit * r.x + sys.origin.x),
        static_cast<int>(sys.unit * r.y + sys.origin.y),
        static_cast<int>(sys.unit * r.w),
        static_cast<int>(sys.unit * r.h)
    };
}

} //namespace libview

#endif
