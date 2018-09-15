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

#ifndef LIBVIEW_RECT_HPP
#define LIBVIEW_RECT_HPP

#include "point.hpp"

namespace libview
{

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

} //namespace libview

#endif
