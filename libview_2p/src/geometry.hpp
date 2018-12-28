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

#ifndef LIBVIEW_2P_GEOMETRY_HPP
#define LIBVIEW_2P_GEOMETRY_HPP

#include <libsdl.hpp>

namespace libview_2p { namespace geometry
{

//
//point
//

struct point
{
    point()
    {
    }

    point(const double x, const double y):
        x(x),
        y(y)
    {
    }

    double x = 0;
    double y = 0;
};

inline
bool operator==(const point& l, const point& r)
{
    return l.x == r.x && l.y == r.y;
}

inline
bool operator!=(const point& l, const point& r)
{
    return !(l == r);
}



//
//rect
//

struct rect
{
    rect()
    {
    }

    rect(const double x, const double y, const double w, const double h):
        pos({x, y}),
        w(w),
        h(h)
    {
    }

    rect(const point& p, const double w, const double h):
        pos(p),
        w(w),
        h(h)
    {
    }

    point pos;
    double w = 0;
    double h = 0;
};

inline
bool operator==(const rect& l, const rect& r)
{
    return l.pos == r.pos && l.w == r.w && l.h == r.h;
}

inline
bool operator!=(const rect& l, const rect& r)
{
    return !(l == r);
}



//
//system
//

//coordinate system
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

}} //namespace libview_2p::geometry

#endif
